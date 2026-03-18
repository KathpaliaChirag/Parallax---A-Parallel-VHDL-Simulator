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
// #include "../core/signal.c"
// #include "../core/process.c"
// #include "../core/scheduler.c"

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

// CHIRAG : dummy run function for now.... 
// later this will be replaced by expression evaluator
// for now just prints that process ran
static void dummy_run(void)
{
    printf("process ran\n");
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
                DYNARRAY_INSERT((*signals), *s)
            }
            break;

        // CHIRAG : arch node.... walk all processes inside it
        case NODE_ARCH:
            printf("walker: processing architecture %s\n", node->data.arch.name);
            for(int i = 0; i < node->data.arch.process_count; i++)
                walk_node(node->data.arch.processes[i], signals, sch);
            break;

        // CHIRAG : process node.... create Process struct
        // add signals from sensitivity list to it
        // add process to scheduler
        case NODE_PROCESS:
            {
                printf("walker: creating process with %d sensitivity signals\n",
                    node->data.process.sensitivity_count);

                // create process with dummy run function for now
                Process p = process_init("vhdl_process", dummy_run);

                // add each sensitivity signal to the process
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

                // add process to scheduler
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