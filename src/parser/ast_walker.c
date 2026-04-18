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
EventQueue* walker_queues[64];
// CHIRAG : walk an expression AST node and compute the result
// reads current signal values from the signals array
static int eval_expr(ASTNode* expr, DynArray_Signal* signals)
{
    if(expr == NULL) return 0;
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
        case EXPR_NOT:
            return !eval_expr(expr->data.expr.left, signals);
        case EXPR_BIT_LITERAL:
            return expr->data.expr.bit_value;
        default: return 0;
    }
}

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

            Event e;
            e.signal_name = target->name;
            e.new_value   = result;
            e.time        = current_time;
            e.delta       = delta + 1;
            e.type        = 0;
            insert_ele(q, e);
            printf("eval: %s <= %d at t=%.1f d=%d\n",
                target->name, result, current_time, delta + 1);
        }
        else if(stmt->type == NODE_IF)
        {
            // CHIRAG 18-04-26 :: if statement ... if CLK = '1' then ... end if
            //
            // how DFF works in VHDL ...
            // process(CLK) ... wakes up when CLK changes
            // if CLK = '1' then Q <= D; end if;
            // so process fires on both rising and falling edge
            // but only updates Q on rising edge (CLK='1')
            // the if statement is what implements edge detection
            //
            // our implementation ...
            // find the condition signal (CLK)
            // check if its current value equals the expected bit value ('1' = 1)
            // if yes ... execute all statements inside the if block
            // if no ... skip ... process did nothing this delta
            Signal* cond_sig = find_signal(ctx->signals, stmt->data.if_stmt.signal_name);
            if(cond_sig == NULL) continue;

            if(cond_sig->value == stmt->data.if_stmt.bit_value)
            {
                // condition true ... execute inner statements
                for(int j = 0; j < stmt->data.if_stmt.statement_count; j++)
                {
                    ASTNode* inner = stmt->data.if_stmt.statements[j];
                    if(inner == NULL || inner->type != NODE_ASSIGN) continue;

                    Signal* target = find_signal(ctx->signals, inner->data.assign.target);
                    if(target == NULL) continue;

                    int result = eval_expr(inner->data.assign.expr, ctx->signals);
                    if(result == target->value) continue;

                    Event e;
                    e.signal_name = target->name;
                    e.new_value   = result;
                    e.time        = current_time;
                    e.delta       = delta + 1;
                    e.type        = 0;
                    insert_ele(q, e);
                    printf("eval: %s <= %d at t=%.1f d=%d\n",
                        target->name, result, current_time, delta + 1);
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
                    node->data.port.direction == DIR_IN ? "in" : "out");
                Signal* s = init_signal(node->data.port.name);
                // 13-04-26 :: okay so i have finally added direction here to be taken
                s->direction = (node->data.port.direction == DIR_OUT) ? 1 : 0;
                DYNARRAY_INSERT((*signals), *s)
            }
            break;

        // CHIRAG : arch node.... walk all processes inside it
        case NODE_ARCH:
            printf("walker: processing architecture %s\n", node->data.arch.name);
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