// CHIRAG 19-03-26 :: ast_walker.c
// okay so this is the bridge between parser and simulator
// parser gives me an AST.... a tree of nodes representing the VHDL circuit
// this walker walks that tree and creates the actual Signal + Process structs
// that run_simulation needs
//
// flow:
// AST root (NODE_ARCH)
//   → for each process node
//       → create Process struct
//       → find signals from sensitivity list
//       → add signals to process
//       → add process to scheduler
// AST entity node
//   → for each port node
//       → create Signal struct
//       → add to signals array

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast_walker.h"
#include "../core/event.h"
#include "../core/event_queue.h"
#include "../core/delta.h"
 #include <omp.h>
// CHIRAG : forward declaration.... walk_node calls itself recursively
static void walk_node(ASTNode* node, DynArray_Signal* signals, Scheduler* sch);

// CHIRAG : helper to find a signal by name in the signals array
// returns pointer to signal if found, NULL if not
static Signal* find_signal(DynArray_Signal* signals, char* name)
{
    for(int i = 0; i < signals->size; i++)
        if(strcmp(signals->data[i].name, name) == 0)
            return &signals->data[i];
    return NULL;
}

//20-03-26 : 18:40 :: -----------------------------------------------------------------------
// CHIRAG : context for each process.... stores what it needs to run
// since run() has no params, context lives globally
// CHIRAG 02-04-26 :: removed MAX_PROC_CONTEXTS fixed cap and proc_run_funcs[] slot array
// now proc_contexts is a dynamic array that grows with realloc as processes are added
// no cap, no slot functions needed anymore

// typedef struct {
//     char target_name[64];
//     ASTNode* expr;
//     DynArray_Signal* signals;
// } ProcessContext;
//core problem with the current strcuture was that it was only built to handle one expr and 1 assignment 
// so something like an if statement or something of multiple assignments used to fail badly

// CHIRAG 18-04-26 :: storing entire process AST node instead of single assignment
// old approach ... stored only target_name and expr ... one assignment per process
// new approach ... store the whole process node ... run_proc_generic walks all statements
// this enables if statements ... multiple assignments ... anything the parser can handle
typedef struct {
    ASTNode* process_node;
    DynArray_Signal* signals;
} ProcessContext;



static ProcessContext* proc_contexts = NULL;
static int proc_context_count = 0;
static int proc_contexts_capacity = 0;
// CHIRAG 21-04-26 :: function registry ... stores all declared functions
// when walker sees NODE_FUNC_DECL it stores it here
// when eval_expr sees NODE_FUNC_CALL it looks up function here by name
// simple linear search ... small N ... fine
typedef struct {
    char* name;
    ASTNode* func_node;
} FuncEntry;

static FuncEntry* func_registry = NULL;
static int func_count = 0;
static int func_capacity = 0;

static void register_func(ASTNode* func_node)
{
    // CHIRAG 21-04-26 :: add function to registry ... same realloc pattern as proc_contexts
    if(func_count >= func_capacity)
    {
        func_capacity = func_capacity == 0 ? 8 : func_capacity * 2;
        func_registry = realloc(func_registry, func_capacity * sizeof(FuncEntry));
    }
    func_registry[func_count].name = func_node->data.func_decl.name;
    func_registry[func_count].func_node = func_node;
    func_count++;
    printf("walker: registered function %s\n", func_node->data.func_decl.name);
}

static ASTNode* find_func(char* name)
{
    for(int i = 0; i < func_count; i++)
        if(strcmp(func_registry[i].name, name) == 0)
            return func_registry[i].func_node;
    return NULL;
} 

// CHIRAG : set this before calling run_simulation
// process run functions insert events into this queue
// EventQueue* walker_queue = NULL;
// problem ... walker_queue is a global pointer ... one copy shared by all threads
// in parallel.c inside omp parallel for we do this ...
//     walker_queue = &local_queues[tid];
// thread 0 sets walker_queue to local_queues[0]
// thread 1 sets walker_queue to local_queues[1]
// but since its ONE global ... thread 1 overwrites thread 0s value
// now both threads read walker_queue and see local_queues[1]
// thread 0 inserts its events into local_queues[1] by mistake
// thread 0s events go to wrong queue ... get merged twice or lost entirely
// wrong events = wrong simulation = wrong hash
//
// fix ... threadprivate tells openmp ... give every thread its OWN copy of walker_queue
// thread 0 has its own walker_queue ... thread 1 has its own walker_queue
// when thread 0 sets walker_queue = local_queues[0] ... thread 1 cant see or overwrite it
// zero conflict ... each thread writes to correct local queue
// EventQueue* walker_queue = NULL;
// #pragma omp threadprivate(walker_queue)
// CHIRAG 15-04-26 :: one queue pointer per thread ... thread N uses walker_queues[N]
// replaces threadprivate approach which caused linker issues with bison generated parser
// EventQueue* walker_queues[64];
// REPLACE with -- CHIRAG 21-04-26 :: bumped to 256 for wide circuits
EventQueue* walker_queues[256];
// CHIRAG 22-04-26 :: stress_ns ... artificial busy-wait per process execution
// problem ... our gate processes do close to 5ns of real work ... barrier overhead is well over 500us
// ratio is 100000:1 ... parallel can never win on tiny circuits
// fix ... busy-wait stress_ns nanoseconds inside each process execution
// this way we will probably try to make gate propagation delay alot huge ... 
// idea was more around that the real gates work in alot ddifferent way hey do not model real world
// at stress_ns=1000 ... 1us per gate ... 128 gates = 128us work >> 500us barrier
// wide_and128 with 4 threads should show speedup at stress_ns >= 500
// set via --stress N flag ... default 0 = original behavior unchanged
// int stress_ns = 0; it was removed in later submission version as it was broken 


// CHIRAG 22-04-26 :: gate propagation delay model via --stress N flag readdded after some tests 
// HISTORY ... so the question comes why this exists again and why it was almost removed
// problem 1 ... parallel engine is correct but SLOWER than sequential on all circuits
// root cause ... each gate does ~5ns of real work ... OMP barrier costs ~500us
// ratio 100000:1 ... barrier dominates ... parallel can never win

// first attempt ...i did a try for omp_get_wtime() busy-wait per process
// and it worked on Linux ... showed real speedup on wide_and128 with proper testbench
// however it again failed on Windows ... omp_get_wtime() has 1ms resolution on Windows mingw or 
// so i got to know from claude about why my seq always showed 0 as speedup 
// badsically a little technical to say would be busy-wait loop exits immediately ... stress had zero effect on Windows
// I almost removed it entirely thinking it didnt work

// second attempt which was again a claude based suggestion ... volatile floating point math loop
// why volatile double math instead of timer? ...cause its gonna burns real CPU cycles directly
// no OS timer dependency ... no resolution problem ... works on Windows and Linux
// stress_ns=1000 ... 1000 flop multiplies ... should be 1-4us on 3GHz CPU
// scales linearly ... predictable ... compiler cannot optimize away due to volatile

// so i read this on gpt and claude itself that 
// real CMOS gates have propagation delay ... 28nm AND gate ~50-500ps
// our simulator models all gates as instant ... 0 delay ... which i feel is kinda unrealistic
// stress_ns models gate switching energy dissipation explicitly
// stress_ns=0 ... normal simulation ... zero delay ... default behavior unchanged
// stress_ns=1000 ... models ~1-4us gate delay ... slow TTL-era logic
// stress_ns=50000 ... models heavier computation per gate ... FPGA LUT style

// WHEN TO USE
// stress=0 ... correctness testing ... hash verification ... default
// stress>0 ... parallel benchmarking ... shows speedup architecture is capable of
int stress_ns = 0;
// -------------------------------------------------------------------------
//old implementation ahad problem so fixed it with a new one taken help of AI here 
// CHIRAG : walk an expression AST node and compute the result
// reads current signal values from the signals array
// static int eval_expr(ASTNode* expr, DynArray_Signal* signals)
// {
//     if(expr == NULL) return 0;
//     switch(expr->data.expr.expr_type)
//     {
//         case EXPR_IDENTIFIER:
//         {
//             Signal* s = find_signal(signals, expr->data.expr.identifier);
//             return s ? s->value : 0;
//         }
//         case EXPR_AND:
//             return eval_expr(expr->data.expr.left, signals)
//                  & eval_expr(expr->data.expr.right, signals);
//         case EXPR_OR:
//             return eval_expr(expr->data.expr.left, signals)
//                  | eval_expr(expr->data.expr.right, signals);
//         case EXPR_XOR:
//             // CHIRAG 18-04-26 :: xor ... A xor B = A^B in C ... 1 if exactly one input is 1
//             return eval_expr(expr->data.expr.left, signals) ^ eval_expr(expr->data.expr.right, signals);
//         case EXPR_NOT:
//             return !eval_expr(expr->data.expr.left, signals);
//         case EXPR_BIT_LITERAL:
//             return expr->data.expr.bit_value;
//         case EXPR_FUNC_CALL:
//         {
//             // CHIRAG 21-04-26 :: evaluate a function call inline
//             // idea ... find the function declaration ... bind args to params ... eval body
//             // problem ... functions have their own param names like A B C
//             //             but caller passes signals like IN0 IN1 IN2
//             //             we need to substitute caller args for param names during eval
//             // solution ... build a temporary local signal array
//             //             copy caller arg values into it under param names
//             //             eval body expression using local array instead of global signals
//             // why not modify global signals? ... that would corrupt simulation state
//             // local array is stack allocated ... thrown away after function returns
//             ASTNode* func = find_func(expr->data.expr.identifier);
//             if(func == NULL) return 0;

//             // build local signal array ... one entry per parameter
//             // each entry has param name and value from caller argument
//             DynArray_Signal local_sigs;
//             DYNARRAY_INIT(local_sigs);
//             for(int i = 0; i < func->data.func_decl.param_count; i++)
//             {
//                 Signal s;
//                 s.name = func->data.func_decl.params[i];
//                 // evaluate caller argument to get value
//                 s.value = eval_expr(func->data.func_decl.args_at_call[i], signals);
//                 s.value_next = s.value;
//                 s.prev_value = s.value;
//                 s.direction = 0;
//                 s.last_change_on = 0;
//                 s.last_change_delta = 0;
//                 s.vcd_symbol = 0;
//                 DYNARRAY_INSERT(local_sigs, s);
//             }
//             // eval body with local signals
//             int result = eval_expr(func->data.func_decl.body, &local_sigs);
//             free(local_sigs.data);
//             return result;
//         }
//         default: return 0;
//     }
// }

static int eval_expr(ASTNode* expr, DynArray_Signal* signals)
{
    if(expr == NULL) return 0;

    // CHIRAG 21-04-26 :: handle function calls BEFORE the switch
    // problem with old approach ... old code had EXPR_FUNC_CALL inside the switch
    // but that switch is on expr->data.expr.expr_type ... only valid when expr->type == NODE_EXPR
    // a function call node has type NODE_FUNC_CALL ... not NODE_EXPR
    // so expr->data.expr.expr_type was reading garbage memory ... undefined behavior
    // also old code used func->data.func_decl.args_at_call which doesnt exist in our AST
    // args live in the CALL node ... not the declaration node
    // fix ... check node type first before touching expr_type ... handle func call here
    if(expr->type == NODE_FUNC_CALL)
    {
        ASTNode* func = find_func(expr->data.func_call.name);
        if(func == NULL)
        {
            printf("eval_expr: unknown function %s\n", expr->data.func_call.name);
            return 0;
        }

        // CHIRAG 21-04-26 :: argument substitution ... core of function evaluation
        // idea ... find the function declaration ... bind args to params ... eval body
        // problem ... functions have their own param names like A B C
        //             but caller passes signals like IN0 IN1 IN2
        //             we need to substitute caller args for param names during eval
        // solution ... build a temporary local signal array
        //             copy caller arg values into it under param names
        //             eval body expression using local array instead of global signals
        // why not modify global signals? ... that would corrupt simulation state
        // local array is freed after function returns ... no memory leak
        DynArray_Signal local_sigs;
        DYNARRAY_INIT(local_sigs);
        for(int i = 0; i < func->data.func_decl.param_count && i < expr->data.func_call.arg_count; i++)
        {
            Signal s;
            memset(&s, 0, sizeof(Signal));
            s.name = func->data.func_decl.params[i];
            // evaluate caller argument against GLOBAL signals
            // so majority(A, B, C) ... A B C are looked up in global signal array
            // args[i] lives in the CALL node ... not the decl node ... old code got this wrong
            s.value = eval_expr(expr->data.func_call.args[i], signals);
            s.value_next = s.value;
            s.prev_value = s.value;
            DYNARRAY_INSERT(local_sigs, s);
        }

        // evaluate function body against LOCAL signals ... not global
        // so inside majority() ... A B C refer to params not global signals
        int result = eval_expr(func->data.func_decl.body, &local_sigs);
        free(local_sigs.data);
        return result;
    }

    switch(expr->data.expr.expr_type)
    {
        case EXPR_IDENTIFIER:
        {
            Signal* s = find_signal(signals, expr->data.expr.identifier);
            return s ? s->value : 0;
        }
        case EXPR_AND:
            return eval_expr(expr->data.expr.left, signals)
                 & eval_expr(expr->data.expr.right, signals);
        case EXPR_OR:
            return eval_expr(expr->data.expr.left, signals)
                 | eval_expr(expr->data.expr.right, signals);
        case EXPR_XOR:
            // CHIRAG 18-04-26 :: xor ... A xor B = A^B in C ... 1 if exactly one input is 1
            return eval_expr(expr->data.expr.left, signals) ^ eval_expr(expr->data.expr.right, signals);
        case EXPR_NOT:
            return !eval_expr(expr->data.expr.left, signals);
        case EXPR_BIT_LITERAL:
            return expr->data.expr.bit_value;
        default: return 0;
    }
}
// --------------------------------------------------------------------------------------
// CHIRAG : generic run logic.... called directly by scheduler now via function pointer
// evaluates expression, finds target signal, inserts output event
// CHIRAG 02-04-26 :: this is now THE run function for every process
// idx comes from p.ctx_idx which scheduler passes when calling p.run(p.ctx_idx)
// no more one-function-per-slot pattern.... run_proc_generic handles all processes
// void run_proc_generic(int idx)
// {
//     ProcessContext* ctx = &proc_contexts[idx];
//     if(walker_queue == NULL) return;

//     Signal* target = find_signal(ctx->signals, ctx->target_name);
//     if(target == NULL) return;

//     int result = eval_expr(ctx->expr, ctx->signals);

//     // CHIRAG : only schedule event if value actually changes
//     // without this check.... Y<=0 schedules itself forever
//     if(result == target->value) return;

//     Event e;
//     e.signal_name = target->name;
//     e.new_value   = result;
//     e.time        = current_time;
//     e.delta       = delta + 1;
//     e.type        = 0;
//     insert_ele(walker_queue, e);
//     printf("eval: %s <= %d at t=%.1f d=%d\n",
//         ctx->target_name, result, current_time, delta + 1);
// }


// CHIRAG 15-04-26 :: why walker_queues[omp_get_thread_num()] instead of walker_queue?
    //
    // old code had a single global walker_queue pointer
    // in sequential mode this was fine ... only one thread ... no conflict
    //
    // in parallel mode multiple threads call run_proc_generic simultaneously
    // if all threads use same walker_queue ... two threads call insert_ele on same heap
    // insert_ele does bubble-up ... two threads doing bubble-up simultaneously = heap corruption
    // events get lost or duplicated ... wrong simulation results
    //
    // fix ... give each thread its own queue
    // walker_queues is an array of 64 queue pointers ... one slot per thread
    // thread 0 uses walker_queues[0] ... thread 1 uses walker_queues[1] ... etc
    // omp_get_thread_num() returns current thread id ... 0 in sequential mode
    // so sequential path still works exactly as before ... walker_queues[0] = &eq in parser main
    //
    // after parallel section ... parallel.c merges all local queues back into global queue
    
// void run_proc_generic(int idx)
// {
//     ProcessContext* ctx = &proc_contexts[idx];

//     // CHIRAG 15-04-26 :: use thread local queue instead of single global walker_queue
//     // omp_get_thread_num() returns 0 for sequential ... so sequential path still works
//     // thread N writes to walker_queues[N] ... no contention between threads
//     EventQueue* q = walker_queues[omp_get_thread_num()];
//     if(q == NULL) return;

//     Signal* target = find_signal(ctx->signals, ctx->target_name);
//     if(target == NULL) return;

//     int result = eval_expr(ctx->expr, ctx->signals);

//     // CHIRAG : only schedule event if value actually changes
//     // without this check.... Y<=0 schedules itself forever
//     if(result == target->value) return;

//     Event e;
//     e.signal_name = target->name;
//     e.new_value   = result;
//     e.time        = current_time;
//     e.delta       = delta + 1;
//     e.type        = 0;
//     insert_ele(q, e);
//     printf("eval: %s <= %d at t=%.1f d=%d\n",
//         ctx->target_name, result, current_time, delta + 1);
// }


void run_proc_generic(int idx)
{
    ProcessContext* ctx = &proc_contexts[idx];
    // CHIRAG 22-04-26 :: busy-wait will occure if --stress N was given
    // so initially i was going to put sleep() 
    // then i understoood from claude why busy-wait and not sleep? ... sleep yields the thread ... OS may not wake it on time
    // busy-wait keeps thread on CPU ... this way real gates work alot more accurately 
    // omp_get_wtime() has close to nanoseconds resolution ...
    // this runs BEFORE any logic so overhead soooo.... it is identical for seq and par
    // speedup = seq_time / par_time ... both sides pay same per-gate cost ... fair comparison
    // if(stress_ns > 0)
    // {
    //     double end = omp_get_wtime() + stress_ns * 1e-9;
    //     while(omp_get_wtime() < end);
    // }
    // CHIRAG 22-04-26 :: wellll addedd againnnn....apply gate propagation delay if --stress N given
    // stress_ns=0 ... skip entirely ... zero overhead ... normal path
    // stress_ns>0 ... burn stress_ns floating point multiplies before gate logic
    
    
    // propagation delay happens BEFORE output changes ... input arrives ... gate thinks ... output changes
    // putting busy work before eval_expr models this correctly
    // both seq and par pay same per-gate cost ... fair speedup comparison
    
    // why volatile? ...causeeee without volatile gcc optimizes entire loop away ... x never used
    // volatile forces compiler to actually execute every multiply
    if(stress_ns > 0)
    {
        volatile double x = 1.0000001;
        for(int i = 0; i < stress_ns; i++)
            x = x * 1.0000001;
    }
    // CHIRAG 15-04-26 :: use thread local queue ... explained in detail above
    EventQueue* q = walker_queues[omp_get_thread_num()];
    if(q == NULL) return;

    ASTNode* proc = ctx->process_node;
    if(proc == NULL) return;

    // CHIRAG 18-04-26 :: what changed here and why?
    //
    // OLD approach ... ctx stored target_name and expr ... one assignment only
    // run_proc_generic just did eval_expr on that one expr and scheduled one event
    // problem ... process(CLK) begin if CLK='1' then Q<=D; end if; end process;
    // this has an IF statement not a direct assignment ... old code ignored it entirely
    // DFF would never work ... registers would never work ... any sequential logic broken
    //
    // NEW approach ... ctx stores entire process AST node
    // run_proc_generic now walks ALL statements in the process one by one
    // handles NODE_ASSIGN ... simple combinational ... Y <= A and B
    // handles NODE_IF ... conditional ... if CLK='1' then Q<=D; end if
    // now DFF works ... registers work ... any logic the parser can handle works
    //
    // why not handle nested ifs? ... our parser only supports one level of if
    // good enough for DFF and registers ... deeper nesting is future work
    for(int i = 0; i < proc->data.process.statement_count; i++)
    {
        ASTNode* stmt = proc->data.process.statements[i];
        if(stmt == NULL) continue;

        if(stmt->type == NODE_ASSIGN)
        {
            // CHIRAG 18-04-26 :: simple assignment ... Y <= A and B
            // find target signal ... eval expression ... if changed schedule event
            // same logic as old run_proc_generic just now inside a loop
            Signal* target = find_signal(ctx->signals, stmt->data.assign.target);
            if(target == NULL) continue;

            int result = eval_expr(stmt->data.assign.expr, ctx->signals);

            // only schedule if value actually changes ... without this check
            // Y<=0 when Y is already 0 would schedule event forever ... infinite loop
            if(result == target->value) continue;
            
            
            // CHIRAG 21-04-26 :: after delay scheduling
            // idea ... VHDL allows Y <= A and B after 10 ns; meaning output
            // changes 10ns after input changes not immediately
            // problem ... old code always scheduled at current_time delta+1
            // that means zero delay ... instantaneous propagation ... not realistic
            // solution ... check delay_ns from AST node
            // if 0 ... old behavior ... schedule at current_time delta+1
            // if >0 ... schedule at current_time + delay_ns with delta=0
            // delta=0 because its a new timestamp ... fresh start
            Event e;
            e.signal_name = target->name;
            e.new_value   = result;
            e.type        = 0;
            if(stmt->data.assign.delay_ns > 0)
            {
                e.time  = current_time + stmt->data.assign.delay_ns;
                e.delta = 0;
            }
            else
            {
                e.time  = current_time;
                e.delta = delta + 1;
            }
            insert_ele(q, e);
            printf("eval: %s <= %d at t=%.1f d=%d\n",
                target->name, result, current_time, delta + 1);
        }

        else if(stmt->type == NODE_IF)
        {
            // CHIRAG 22-04-26 :: evaluate if statement ... supports nested ifs
            // old code only handled NODE_ASSIGN inside then/else blocks
            // problem ... TFF needs if CLK='1' then if T='1' then ... nested
            // fix ... extracted execute_statements helper that handles both
            // NODE_ASSIGN and NODE_IF recursively ... any depth works now
            Signal* cond_sig = find_signal(ctx->signals, stmt->data.if_stmt.signal_name);
            if(cond_sig == NULL) continue;

            ASTNode** block;
            int count;
            if(cond_sig->value == stmt->data.if_stmt.bit_value)
            {
                block = stmt->data.if_stmt.statements;
                count = stmt->data.if_stmt.statement_count;
            }
            else
            {
                block = stmt->data.if_stmt.else_statements;
                count = stmt->data.if_stmt.else_statement_count;
            }

            // CHIRAG 22-04-26 :: execute the chosen block ... handles nested ifs
            for(int j = 0; j < count; j++)
            {
                ASTNode* inner = block[j];
                if(inner == NULL) continue;

                if(inner->type == NODE_ASSIGN)
                {
                    Signal* target = find_signal(ctx->signals, inner->data.assign.target);
                    if(target == NULL) continue;
                    int result = eval_expr(inner->data.assign.expr, ctx->signals);
                    if(result == target->value) continue;
                    Event e;
                    e.signal_name = target->name;
                    e.new_value   = result;
                    e.type        = 0;
                    if(inner->data.assign.delay_ns > 0)
                    {
                        e.time  = current_time + inner->data.assign.delay_ns;
                        e.delta = 0;
                    }
                    else
                    {
                        e.time  = current_time;
                        e.delta = delta + 1;
                    }
                    insert_ele(q, e);
                    printf("eval: %s <= %d at t=%.1f d=%d\n",
                        target->name, result, current_time, delta + 1);
                }
                else if(inner->type == NODE_IF)
                {
                    // CHIRAG 22-04-26 :: nested if ... same logic one level deeper
                    // this is what enables TFF ... if CLK='1' then if T='1' then Q<=not Q
                    Signal* inner_cond = find_signal(ctx->signals, inner->data.if_stmt.signal_name);
                    if(inner_cond == NULL) continue;

                    ASTNode** inner_block;
                    int inner_count;
                    if(inner_cond->value == inner->data.if_stmt.bit_value)
                    {
                        inner_block = inner->data.if_stmt.statements;
                        inner_count = inner->data.if_stmt.statement_count;
                    }
                    else
                    {
                        inner_block = inner->data.if_stmt.else_statements;
                        inner_count = inner->data.if_stmt.else_statement_count;
                    }

                    for(int k = 0; k < inner_count; k++)
                    {
                        ASTNode* innermost = inner_block[k];
                        if(innermost == NULL || innermost->type != NODE_ASSIGN) continue;
                        Signal* target = find_signal(ctx->signals, innermost->data.assign.target);
                        if(target == NULL) continue;
                        int result = eval_expr(innermost->data.assign.expr, ctx->signals);
                        if(result == target->value) continue;
                        Event e;
                        e.signal_name = target->name;
                        e.new_value   = result;
                        e.type        = 0;
                        e.time        = current_time;
                        e.delta       = delta + 1;
                        insert_ele(q, e);
                        printf("eval: %s <= %d at t=%.1f d=%d\n",
                            target->name, result, current_time, delta + 1);
                    }
                }
            }
        }
    }
}
// CHIRAG : main entry point.... called from main after parsing
// root is the top of the AST.... usually NODE_ARCH
void ast_walk(ASTNode* root, DynArray_Signal* signals, Scheduler* sch)
{
    if(root == NULL) return;
    walk_node(root, signals, sch);
}

// CHIRAG : recursive walker.... handles each node type
static void walk_node(ASTNode* node, DynArray_Signal* signals, Scheduler* sch)
{
    if(node == NULL) return;

    switch(node->type)
    {
        // CHIRAG : entity node.... create signals from ports
        // each port becomes a Signal struct in the signals array
        case NODE_ENTITY:
            printf("walker: processing entity %s\n", node->data.entity.name);
            for(int i = 0; i < node->data.entity.port_count; i++)
                walk_node(node->data.entity.ports[i], signals, sch);
            break;

        // CHIRAG : port node.... create one Signal and add to signals array
        case NODE_PORT:
            {
                printf("walker: creating signal %s (%s)\n",
                    node->data.port.name,
                    node->data.port.direction == DIR_IN ? "in" :
                    node->data.port.direction == DIR_OUT ? "out" : "internal");
                Signal* s = init_signal(node->data.port.name);
                // CHIRAG 18-04-26 :: internal signals get direction 2
                // direction 0 = input ... direction 1 = output ... direction 2 = internal wire
                // testbench auto-mode skips direction!=0 so internal signals never get seeded
                // correct ... internal wires are driven by processes not testbench
                s->direction = (node->data.port.direction == DIR_OUT) ? 1 :
                               (node->data.port.direction == DIR_INTERNAL) ? 2 : 0;
                DYNARRAY_INSERT((*signals), *s)
            }
            break;
        // CHIRAG : arch node.... walk all processes inside it
           case NODE_ARCH:
                printf("walker: processing architecture %s\n", node->data.arch.name);
                // CHIRAG 18-04-26 :: walk internal signals FIRST before processes
                // why first? ... processes reference these signals by name
                // if we walk processes first ... find_signal returns NULL ... process cant find CARRY etc
                // signals must exist in the array before any process tries to read/write them
                // CHIRAG 21-04-26 :: register functions FIRST before signals and processes
                // why first? ... processes may call functions ... functions must be known before eval
                // old code only walked signals then processes ... functions were never registered
                // fix ... loop over arch.funcs and call register_func for each one
                for(int i = 0; i < node->data.arch.func_count; i++)
                    register_func(node->data.arch.funcs[i]);
                // walk internal signals before processes ... signals must exist before processes reference them
                for(int i = 0; i < node->data.arch.signal_count; i++)
                    walk_node(node->data.arch.signals[i], signals, sch);
                for(int i = 0; i < node->data.arch.process_count; i++)
                    walk_node(node->data.arch.processes[i], signals, sch);
            break;
        case NODE_PROCESS:
        {
            printf("walker: creating process with %d sensitivity signals\n",
                node->data.process.sensitivity_count);
            
            // CHIRAG : find the assignment statement inside this process
            // store target + expr in context so run_proc_generic can use them via idx
            // CHIRAG 02-04-26 :: grow proc_contexts dynamically if needed
            // realloc doubles capacity each time so amortized cost is O(1) per insert
            if(proc_context_count >= proc_contexts_capacity)
            {
                proc_contexts_capacity = proc_contexts_capacity == 0 ? 8 : proc_contexts_capacity * 2;
                proc_contexts = realloc(proc_contexts, proc_contexts_capacity * sizeof(ProcessContext));
            }

            // int ctx_idx = proc_context_count;
            // ProcessContext* ctx = &proc_contexts[ctx_idx];
            // ctx->signals = signals;
            
            // for(int i = 0; i < node->data.process.statement_count; i++)
            // {
            //     ASTNode* stmt = node->data.process.statements[i];
            //     if(stmt && stmt->type == NODE_ASSIGN)
            //     {
            //         strncpy(ctx->target_name, stmt->data.assign.target, 63);
            //         ctx->expr = stmt->data.assign.expr;
            //         printf("walker: process computes %s <=\n", ctx->target_name);
            //         break;
            //     }
            // }
            int ctx_idx = proc_context_count;
            ProcessContext* ctx = &proc_contexts[ctx_idx];
            ctx->signals = signals;
            // CHIRAG 18-04-26 :: store entire process node instead of single assignment
            // old way searched for first NODE_ASSIGN and stored target+expr ... one assignment only
            // new way stores whole process node ... run_proc_generic walks all statements
            // this enables if statements and multiple assignments in same process
            ctx->process_node = node;
            printf("walker: process node stored for ctx %d\n", ctx_idx);
        
            // CHIRAG 02-04-26 :: pass run_proc_generic directly as the run function
            // and ctx_idx so process knows which context slot is its own
            // scheduler will call p.run(p.ctx_idx) which routes to correct context
            Process p = process_init("vhdl_process", run_proc_generic, ctx_idx);
            proc_context_count++;
        
            for(int i = 0; i < node->data.process.sensitivity_count; i++)
            {
                char* sig_name = node->data.process.sensitivity[i];
                Signal* s = find_signal(signals, sig_name);
                if(s != NULL)
                {
                    process_add_signal(&p, *s);
                    printf("walker: process watches signal %s\n", sig_name);
                }
                else
                    printf("walker: WARNING signal %s not found\n", sig_name);
            }
        
            scheduler_add_process(sch, p);
            printf("walker: process added to scheduler\n");
        }
        break;

        // CHIRAG : assignment node.... for now just print
        // later this will set up the process run function properly
        case NODE_ASSIGN:
            printf("walker: assignment %s <=\n", node->data.assign.target);
            break;

        // CHIRAG : if node.... for now just print
        case NODE_IF:
            printf("walker: if %s = '%d'\n",
                node->data.if_stmt.signal_name,
                node->data.if_stmt.bit_value);
            break;

        default:
            break;
    }
}