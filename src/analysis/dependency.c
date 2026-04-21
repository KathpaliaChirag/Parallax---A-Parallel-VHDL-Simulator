#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dependency.h"

// #define MAX_PROC 64
// #define MAX_SIG 64
// CHIRAG 21-04-26 :: bumped from 64 to 256 ... wide_and128 needs 128+ processes
#define MAX_PROC 256
#define MAX_SIG 256
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
    // CHIRAG 18-04-26 :: changed writes from single string to array
    // old code ... writes[i] was one char[64] ... only stored first assignment target
    // problem ... process with two assignments like SUM0 <= ... CARRY0 <=
    // old code saw SUM0 and broke ... CARRY0 never recorded as a write
    // so proc0-proc1 edge on CARRY0 was never added ... graph wrong ... parallel mode unsafe
    // fix ... writes[i] is now an array just like reads[i] ... collect ALL targets
    char writes[MAX_PROC][MAX_SIG][64];
    int write_count[MAX_PROC];
    char reads[MAX_PROC][MAX_SIG][64];
    int read_count[MAX_PROC];

    for(int i = 0; i < MAX_PROC; i++)
    {
        write_count[i] = 0;
        read_count[i] = 0;
    }

    if(root == NULL || root->type != NODE_ARCH) return;

    int num_procs = root->data.arch.process_count;

    for(int i = 0; i < num_procs; i++)
    {
        ASTNode* proc = root->data.arch.processes[i];
        if(proc == NULL || proc->type != NODE_PROCESS) continue;

        for(int s = 0; s < proc->data.process.statement_count; s++)
        {
            ASTNode* stmt = proc->data.process.statements[s];
            if(stmt == NULL) continue;

            if(stmt->type == NODE_ASSIGN)
            {
                // CHIRAG 18-04-26 :: collect ALL assignments ... no break anymore
                // every target is a write ... every expr identifier is a read
                strncpy(writes[i][write_count[i]], stmt->data.assign.target, 63);
                write_count[i]++;
                collect_reads(stmt->data.assign.expr, reads[i], &read_count[i]);
            }
            else if(stmt->type == NODE_IF)
            {
                // CHIRAG 18-04-26 :: also collect writes/reads inside if blocks
                // if CLK='1' then Q<=D; end if ... Q is a write, D is a read
                // old code ignored if blocks entirely ... DFF had no edges in graph
                for(int j = 0; j < stmt->data.if_stmt.statement_count; j++)
                {
                    ASTNode* inner = stmt->data.if_stmt.statements[j];
                    if(inner == NULL || inner->type != NODE_ASSIGN) continue;
                    strncpy(writes[i][write_count[i]], inner->data.assign.target, 63);
                    write_count[i]++;
                    collect_reads(inner->data.assign.expr, reads[i], &read_count[i]);
                }
            }
        }
    }

    // CHIRAG 18-04-26 :: conflict check now loops over ALL writes not just one
    // same logic as before ... i writes what j reads = conflict ... add edge
    for(int i = 0; i < num_procs; i++)
    {
        for(int j = i+1; j < num_procs; j++)
        {
            int conflict = 0;

            for(int wi = 0; wi < write_count[i]; wi++)
                for(int r = 0; r < read_count[j]; r++)
                    if(strcmp(writes[i][wi], reads[j][r]) == 0)
                        conflict = 1;

            for(int wj = 0; wj < write_count[j]; wj++)
                for(int r = 0; r < read_count[i]; r++)
                    if(strcmp(writes[j][wj], reads[i][r]) == 0)
                        conflict = 1;

            for(int wi = 0; wi < write_count[i]; wi++)
                for(int wj = 0; wj < write_count[j]; wj++)
                    if(strcmp(writes[i][wi], writes[j][wj]) == 0)
                        conflict = 1;

            if(conflict)
                graph_add_edge(g, i, j);
        }
    }

    graph_color(g);
    graph_print(g);
}