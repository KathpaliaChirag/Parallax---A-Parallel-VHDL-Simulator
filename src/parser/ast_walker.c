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

typedef struct {
    char target_name[64];
    ASTNode* expr;
    DynArray_Signal* signals;
} ProcessContext;

static ProcessContext* proc_contexts = NULL;
static int proc_context_count = 0;
static int proc_contexts_capacity = 0;

// CHIRAG : set this before calling run_simulation
// process run functions insert events into this queue
EventQueue* walker_queue = NULL;

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
void run_proc_generic(int idx)
{
    ProcessContext* ctx = &proc_contexts[idx];
    if(walker_queue == NULL) return;

    Signal* target = find_signal(ctx->signals, ctx->target_name);
    if(target == NULL) return;

    int result = eval_expr(ctx->expr, ctx->signals);

    // CHIRAG : only schedule event if value actually changes
    // without this check.... Y<=0 schedules itself forever
    if(result == target->value) return;

    Event e;
    e.signal_name = target->name;
    e.new_value   = result;
    e.time        = current_time;
    e.delta       = delta + 1;
    e.type        = 0;
    insert_ele(walker_queue, e);
    printf("eval: %s <= %d at t=%.1f d=%d\n",
        ctx->target_name, result, current_time, delta + 1);
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

            int ctx_idx = proc_context_count;
            ProcessContext* ctx = &proc_contexts[ctx_idx];
            ctx->signals = signals;
            
            for(int i = 0; i < node->data.process.statement_count; i++)
            {
                ASTNode* stmt = node->data.process.statements[i];
                if(stmt && stmt->type == NODE_ASSIGN)
                {
                    strncpy(ctx->target_name, stmt->data.assign.target, 63);
                    ctx->expr = stmt->data.assign.expr;
                    printf("walker: process computes %s <=\n", ctx->target_name);
                    break;
                }
            }
        
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