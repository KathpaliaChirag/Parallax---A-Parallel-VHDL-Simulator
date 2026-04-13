#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dependency.h"

#define MAX_PROC 64
#define MAX_SIG 64

// CHIRAG 04-04-26 :: recursively walk expression and collect all signal names read
// expr like (A and B) ... reads both A and B
// stores found names into reads[proc_idx]
static void collect_reads(ASTNode* expr, char reads[][64], int* count)
{
    if(expr == NULL) return;
    switch(expr->data.expr.expr_type)
    {
        case EXPR_IDENTIFIER:
            strncpy(reads[*count], expr->data.expr.identifier, 63);
            (*count)++;
            break;
        case EXPR_AND:
        case EXPR_OR:
            collect_reads(expr->data.expr.left, reads, count);
            collect_reads(expr->data.expr.right, reads, count);
            break;
        case EXPR_NOT:
            collect_reads(expr->data.expr.left, reads, count);
            break;
        default: break;
    }
}

void dependency_extract(ASTNode* root, DepGraph* g)
{
    // CHIRAG 04-04-26 :: read and write sets for each process
    // writes[i] = signal name that process i writes to ... one per process since each has one assignment
    // reads[i] = all signal names process i reads from ... can be multiple (A and B and C etc)
    char writes[MAX_PROC][64];
    char reads[MAX_PROC][MAX_SIG][64];
    int read_count[MAX_PROC];

    // init everything to empty
    for(int i = 0; i < MAX_PROC; i++)
    {
        writes[i][0] = '\0';
        read_count[i] = 0;
    }

    // CHIRAG 04-04-26 :: walk arch node to find all processes
    // root should be NODE_ARCH ... processes live inside it
    if(root == NULL || root->type != NODE_ARCH) return;

    int num_procs = root->data.arch.process_count;

    for(int i = 0; i < num_procs; i++)
    {
        ASTNode* proc = root->data.arch.processes[i];
        if(proc == NULL || proc->type != NODE_PROCESS) continue;

        // extract write set ... target of the assignment statement
        for(int s = 0; s < proc->data.process.statement_count; s++)
        {
            ASTNode* stmt = proc->data.process.statements[s];
            if(stmt && stmt->type == NODE_ASSIGN)
            {
                strncpy(writes[i], stmt->data.assign.target, 63);
                // extract read set ... all identifiers in the expression
                collect_reads(stmt->data.assign.expr, reads[i], &read_count[i]);
                break;
            }
        }
    }

    // CHIRAG 04-04-26 :: now check every pair of processes for conflicts
    // process i and j conflict if i writes what j reads or writes ... or vice versa
    for(int i = 0; i < num_procs; i++)
    {
        for(int j = i+1; j < num_procs; j++)
        {
            int conflict = 0;

            // does i write what j reads?
            for(int r = 0; r < read_count[j]; r++)
                if(strcmp(writes[i], reads[j][r]) == 0)
                    conflict = 1;

            // does j write what i reads?
            for(int r = 0; r < read_count[i]; r++)
                if(strcmp(writes[j], reads[i][r]) == 0)
                    conflict = 1;

            // do both write same signal?
            if(strcmp(writes[i], writes[j]) == 0)
                conflict = 1;

            if(conflict)
                graph_add_edge(g, i, j);
        }
    }

    graph_color(g);
    graph_print(g);
}