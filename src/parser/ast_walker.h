#ifndef AST_WALKER_H
#define AST_WALKER_H
// #include "lexer.l"
// #include "lexer.h"
#include "parser.h"
#include "../core/event_queue.h"
#include "ast.h"
// #include "ast.c"
#include "../core/signal.h"
#include "../core/process.h"
#include "../core/scheduler.h"
#include "../core/utils.h"
// CHIRAG 15-04-26 :: exposed so parallel.c can access and set per-thread queue
// extern EventQueue* walker_queue;
// CHIRAG 15-04-26 :: array of queues ... one per thread ... indexed by omp_get_thread_num()
// avoids threadprivate linker issues with bison generated code
extern EventQueue* walker_queues[64];
void ast_walk(ASTNode* root, DynArray_Signal* signals, Scheduler* sch);
// CHIRAG 02-04-26 06:07 :: exposed so parallel.c can use it directly as thread entry point
void run_proc_generic(int idx);
#endif