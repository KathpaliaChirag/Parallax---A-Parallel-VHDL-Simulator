%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast_walker.h"
#include "../core/scheduler.h"
#include "../core/signal.h"
#include "../core/utils.h"
#include "../output/vcd.h"
#include "../analysis/dependency.h"

#include "../core/event.h"
#include "../sim/sequential.h"
#include "../core/delta.h"
#include "../core/event_queue.h"
extern EventQueue* walker_queue;

void yyerror(const char* s);
int yylex();
extern char* yytext;
struct ASTNode;
extern struct ASTNode* ast_root;
extern struct ASTNode* ast_entity;

// CHIRAG 19-03-26 :: temporary storage during parsing
// ports and processes are collected here as they are parsed
// then copied into the entity/arch node when that rule completes
ASTNode* temp_ports[32];
ASTNode* temp_stmts[32];
int temp_stmt_count = 0;
int temp_port_count = 0;
ASTNode* temp_processes[32];
int temp_process_count = 0;
char* temp_sens[32];
int temp_sens_count = 0;
%}

%code requires {
    #include "ast.h"
}

%union {
    char* str;
    int num;
    ASTNode* node;
}

%token <str> IDENTIFIER
%token <num> ZERO ONE
%token ENTITY ARCHITECTURE PORT PROCESS
%token BEGIN_TOK END_TOK IS OF
%token IN_TOK OUT_TOK BIT
%token AND_TOK OR_TOK NOT_TOK
%token IF THEN SIGNAL
%token ASSIGN

%type <node> program entity_decl architecture_decl
%type <node> process_decl statement signal_assignment if_statement
%type <node> expression port_item port_list process_list statement_list
%type <str>  identifier_list
%type <num>  bit_literal

%left OR_TOK
%left AND_TOK
%right NOT_TOK

%%

program
    : entity_decl architecture_decl
         {
            ast_entity = $1;  
            $$ = $2;
            ast_root = $$;
            ast_print(ast_root, 0);
        }
    ;

entity_decl
    : ENTITY IDENTIFIER IS PORT '(' port_list ')' ';' END_TOK IDENTIFIER ';'
        {
            $$ = ast_new_node(NODE_ENTITY);
            $$->data.entity.name = strdup($2);
            // CHIRAG : copy collected ports into entity node
            for(int i = 0; i < temp_port_count; i++)
                $$->data.entity.ports[i] = temp_ports[i];
            $$->data.entity.port_count = temp_port_count;
            temp_port_count = 0;
            printf("parsed entity: %s with %d ports\n", $2, $$->data.entity.port_count);
        }
    ;

port_list
    : port_item
        { $$ = $1; }
    | port_list ';' port_item
        { $$ = $3; }
    ;

port_item
    : identifier_list ':' IN_TOK BIT
        {
            $$ = ast_new_node(NODE_PORT);
            $$->data.port.name = strdup($1);
            $$->data.port.direction = DIR_IN;
            temp_ports[temp_port_count++] = $$;
            temp_sens_count = 0;  // ← add this
            printf("parsed input port: %s\n", $1);
        }
    | identifier_list ':' OUT_TOK BIT
        {
            $$ = ast_new_node(NODE_PORT);
            $$->data.port.name = strdup($1);
            $$->data.port.direction = DIR_OUT;
            temp_ports[temp_port_count++] = $$;
            temp_sens_count = 0;  // ← add this
            printf("parsed output port: %s\n", $1);
        }
    ;

identifier_list
    : IDENTIFIER
        { $$ = $1; temp_sens[temp_sens_count++] = strdup($1); }
    | identifier_list ',' IDENTIFIER
        { $$ = $1; temp_sens[temp_sens_count++] = strdup($3); }
    ;

architecture_decl
    : ARCHITECTURE IDENTIFIER OF IDENTIFIER IS BEGIN_TOK process_list END_TOK IDENTIFIER ';'
        {
            $$ = ast_new_node(NODE_ARCH);
            $$->data.arch.name = strdup($2);
            $$->data.arch.entity_name = strdup($4);
            // CHIRAG : copy collected processes into arch node
            for(int i = 0; i < temp_process_count; i++)
                $$->data.arch.processes[i] = temp_processes[i];
            $$->data.arch.process_count = temp_process_count;
            temp_process_count = 0;
            printf("parsed architecture: %s of %s with %d processes\n", $2, $4, $$->data.arch.process_count);
        }
    ;

process_list
    : process_decl
        { $$ = $1; }
    | process_list process_decl
        { $$ = $2; }
    ;

process_decl
    : PROCESS '(' identifier_list ')' BEGIN_TOK statement_list END_TOK PROCESS ';'
        {
            $$ = ast_new_node(NODE_PROCESS);
            // CHIRAG : copy all sensitivity signals, not just first one
            for(int i = 0; i < temp_sens_count; i++)
                $$->data.process.sensitivity[i] = temp_sens[i];
            $$->data.process.sensitivity_count = temp_sens_count;
            temp_sens_count = 0;
            for(int i = 0; i < temp_stmt_count; i++)
                $$->data.process.statements[i] = temp_stmts[i];
            $$->data.process.statement_count = temp_stmt_count;
            temp_stmt_count = 0;
            temp_processes[temp_process_count++] = $$;
            printf("parsed process with sensitivity: %d signals, statements: %d\n",
                $$->data.process.sensitivity_count, $$->data.process.statement_count);
        }
    ;

statement_list
    : statement
        { $$ = $1; }
    | statement_list statement
        { $$ = $2; }
    ;

statement
    : signal_assignment
        { $$ = $1; temp_stmts[temp_stmt_count++] = $1; }
    | if_statement
        { $$ = $1; temp_stmts[temp_stmt_count++] = $1; }
    ;

signal_assignment
    : IDENTIFIER ASSIGN expression ';'
        {
            $$ = ast_new_node(NODE_ASSIGN);
            $$->data.assign.target = strdup($1);
            $$->data.assign.expr = $3;
            printf("parsed assignment: %s <=\n", $1);
        }
    ;

if_statement
    : IF IDENTIFIER '=' bit_literal THEN statement_list END_TOK IF ';'
        {
            $$ = ast_new_node(NODE_IF);
            $$->data.if_stmt.signal_name = strdup($2);
            $$->data.if_stmt.bit_value = $4;
            printf("parsed if: %s = '%d'\n", $2, $4);
        }
    ;

bit_literal
    : ZERO  { $$ = 0; }
    | ONE   { $$ = 1; }
    ;

expression
    : IDENTIFIER
        {
            $$ = ast_new_node(NODE_EXPR);
            $$->data.expr.expr_type = EXPR_IDENTIFIER;
            $$->data.expr.identifier = strdup($1);
        }
    | expression AND_TOK expression
        {
            $$ = ast_new_node(NODE_EXPR);
            $$->data.expr.expr_type = EXPR_AND;
            $$->data.expr.left = $1;
            $$->data.expr.right = $3;
        }
    | expression OR_TOK expression
        {
            $$ = ast_new_node(NODE_EXPR);
            $$->data.expr.expr_type = EXPR_OR;
            $$->data.expr.left = $1;
            $$->data.expr.right = $3;
        }
    | NOT_TOK expression
        {
            $$ = ast_new_node(NODE_EXPR);
            $$->data.expr.expr_type = EXPR_NOT;
            $$->data.expr.left = $2;
        }
    | '(' expression ')'
        { $$ = $2; }
    ;

%%

void yyerror(const char* s)
{
    fprintf(stderr, "parse error: %s\n", s);
}

int main(int argc, char* argv[])
{
    int result = yyparse();
    if(result != 0) { printf("parsing failed\n"); return 1; }
    printf("parsing done! walking AST now...\n");

    DynArray_Signal signals;
    DYNARRAY_INIT(signals)
    Scheduler sch = scheduler_init();

    ast_walk(ast_entity, &signals, &sch);
    ast_walk(ast_root, &signals, &sch);
    printf("AST walk done! %d signals created\n", signals.size);

    // CHIRAG 04-04-26 :: build dependency graph and color it
    // process count comes from scheduler after ast_walk populates it
    DepGraph* g = graph_build(sch.process_ARRAY.size);
    dependency_extract(ast_root, g);
    graph_free(g);

    EventQueue eq = init_queue();
    walker_queue = &eq;

    Event e;
    e.type = 0; e.delta = 0;

    e.signal_name = "A"; e.new_value = 0; e.time = 0; insert_ele(&eq, e);
    e.signal_name = "B"; e.new_value = 0; e.time = 0; insert_ele(&eq, e);
    e.signal_name = "A"; e.new_value = 1; e.time = 1; insert_ele(&eq, e);
    e.signal_name = "B"; e.new_value = 1; e.time = 2; insert_ele(&eq, e);
    e.signal_name = "A"; e.new_value = 0; e.time = 3; insert_ele(&eq, e);
    e.signal_name = "B"; e.new_value = 0; e.time = 4; insert_ele(&eq, e);
    e.signal_name = "A"; e.new_value = 1; e.time = 5; insert_ele(&eq, e);
    e.signal_name = "B"; e.new_value = 0; e.time = 7; insert_ele(&eq, e);
    e.signal_name = "A"; e.new_value = 1; e.time = 9; insert_ele(&eq, e);
    e.signal_name = "B"; e.new_value = 1; e.time = 9; insert_ele(&eq, e);

    char vcd_name[64];
    if(argc > 1)
        snprintf(vcd_name, 64, "output-%s.vcd", argv[1]);
    else
        snprintf(vcd_name, 64, "output-parser.vcd");

    vcd_init(vcd_name);
    vcd_write_header(&signals);
    init_run();
    run_simulation(&eq, &sch, &signals);

    printf("\nfinal signal values:\n");
    for(int i = 0; i < signals.size; i++)
        printf("  %s = %d\n", signals.data[i].name, signals.data[i].value);

    return 0;
}