#ifndef AST_WALKER_H
#define AST_WALKER_H
// #include "lexer.l"
// #include "lexer.h"
#include "parser.h"
#include "ast.h"
// #include "ast.c"
#include "../core/signal.h"
#include "../core/process.h"
#include "../core/scheduler.h"
#include "../core/utils.h"

void ast_walk(ASTNode* root, DynArray_Signal* signals, Scheduler* sch);
// CHIRAG 02-04-26 06:07 :: exposed so parallel.c can use it directly as thread entry point
void run_proc_generic(int idx);
#endif