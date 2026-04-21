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
#include "../output/trace.h"
#include "../output/vcd.h"
#include "../sim/parallel.h"
#include "../core/event.h"
#include "../sim/sequential.h"
#include "../core/delta.h"
#include "../core/event_queue.h"
#include <omp.h>

void yyerror(const char* s);
int yylex();
extern char* yytext;
struct ASTNode;
extern struct ASTNode* ast_root;
extern struct ASTNode* ast_entity;

// CHIRAG 19-03-26 :: temporary storage during parsing
// ports and processes are collected here as they are parsed
// then copied into the entity/arch node when that rule completes
ASTNode* temp_ports[256];
ASTNode* temp_stmts[256];
int temp_stmt_count = 0;
int temp_port_count = 0;
ASTNode* temp_processes[256];
int temp_process_count = 0;
char* temp_sens[256];
int temp_sens_count = 0;
// CHIRAG 18-04-26 :: temp storage for internal signal declarations
// signal CARRY : bit; between IS and BEGIN goes here
// separate from temp_ports ... ports belong to entity ... these belong to arch
ASTNode* temp_arch_signals[256];
int temp_arch_signal_count = 0;
// CHIRAG 21-04-26 :: temp storage for function declarations
// functions live between IS and BEGIN in architecture ... like signal decls
// stored here during parsing ... copied into arch node when rule completes
ASTNode* temp_funcs[128];
int temp_func_count = 0;

// temp storage for function parameters during parsing
char* temp_params[128];
int temp_param_count = 0;

// temp storage for function call arguments during parsing
ASTNode* temp_args[128];
int temp_arg_count = 0;
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
%token AND_TOK OR_TOK NOT_TOK XOR_TOK
%token IF THEN SIGNAL ELSE_TOK
%token ASSIGN
%token AFTER_TOK NS_TOK
%token FUNCTION_TOK RETURN_TOK
%token <num> INTEGER
%type <node> program entity_decl architecture_decl
%type <node> process_decl statement signal_assignment if_statement
%type <node> expression port_item port_list process_list statement_list
%type <node> signal_decl signal_decl_list
%type <node> func_decl func_decl_list func_param_list func_param
%type <node> arg_list
%type <num>  bit_literal
%type <str> identifier_list

%left OR_TOK
%left AND_TOK XOR_TOK
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
func_decl_list
    : func_decl
        { $$ = $1; }
    | func_decl_list func_decl
        { $$ = $2; }
    ;

func_decl
    : FUNCTION_TOK IDENTIFIER '(' func_param_list ')' RETURN_TOK BIT IS BEGIN_TOK RETURN_TOK expression ';' END_TOK FUNCTION_TOK ';'
        {
            // CHIRAG 21-04-26 :: function declaration
            // idea ... support function majority(A:bit; B:bit) return bit is begin return expr; end function;
            // problem ... functions need their own AST node to store params and body
            // solution ... NODE_FUNC_DECL stores name, param names, and body expression
            // only single-return functions supported ... no local variables ... no loops
            // this covers 90% of useful utility functions in RTL design
            $$ = ast_new_node(NODE_FUNC_DECL);
            $$->data.func_decl.name = strdup($2);
            $$->data.func_decl.param_count = temp_param_count;
            for(int i = 0; i < temp_param_count; i++)
                $$->data.func_decl.params[i] = strdup(temp_params[i]);
            // CHIRAG 21-04-26 :: was $12 before ... counted tokens wrong
            // FUNCTION_TOK($1) IDENTIFIER($2) '('($3) func_param_list($4) ')'($5) 
            // RETURN_TOK($6) BIT($7) IS($8) BEGIN_TOK($9) RETURN_TOK($10) expression($11)
            // so expression is $11 not $12 ... $12 is the semicolon ... no type on semicolons
            $$->data.func_decl.body = $11;
            // $$->data.func_decl.body = $12;
            temp_param_count = 0;
            temp_funcs[temp_func_count++] = $$;
            printf("parsed function: %s with %d params\n", $2, $$->data.func_decl.param_count);
        }
    ;

func_param_list
    : func_param
        { $$ = $1; }
    | func_param_list ';' func_param
        { $$ = $3; }
    ;

func_param
    : IDENTIFIER ':' BIT
        {
            // CHIRAG 21-04-26 :: one function parameter ... A : bit
            // store name in temp_params ... func_decl rule copies them
            $$ = ast_new_node(NODE_PORT);
            $$->data.port.name = strdup($1);
            temp_params[temp_param_count++] = strdup($1);
            printf("parsed func param: %s\n", $1);
        }
    ;

arg_list
    : expression
        {
            $$ = $1;
            temp_args[temp_arg_count++] = $1;
        }
    | arg_list ',' expression
        {
            $$ = $3;
            temp_args[temp_arg_count++] = $3;
        }
    ;
signal_decl_list
    : signal_decl
        { $$ = $1; }
    | signal_decl_list signal_decl
        { $$ = $2; }
    ;

signal_decl
    : SIGNAL IDENTIFIER ':' BIT ';'
        {
            // CHIRAG 18-04-26 :: internal signal ... lives in arch not entity
            // reusing NODE_PORT with DIR_INTERNAL so walker can create Signal struct
            // same logic as port ... just different direction tag
            $$ = ast_new_node(NODE_PORT);
            $$->data.port.name = strdup($2);
            $$->data.port.direction = DIR_INTERNAL;
            temp_arch_signals[temp_arch_signal_count++] = $$;
            printf("parsed internal signal: %s\n", $2);
        }
    ;

identifier_list
    : IDENTIFIER
        { $$ = $1; temp_sens[temp_sens_count++] = strdup($1); }
    | identifier_list ',' IDENTIFIER
        { $$ = $1; temp_sens[temp_sens_count++] = strdup($3); }
    ;


process_list
    : process_decl
        { $$ = $1; }
    | process_list process_decl
        { $$ = $2; }
    ;

architecture_decl
    : ARCHITECTURE IDENTIFIER OF IDENTIFIER IS signal_decl_list func_decl_list BEGIN_TOK process_list END_TOK IDENTIFIER ';'
        {
            // CHIRAG 21-04-26 :: signals + functions + processes ... most complete variant
            // problem ... old architecture_decl only had 2 variants ... with signals and without
            // it never referenced func_decl_list so bison marked those rules as useless
            // fix ... added 4 variants to cover all combinations
            // variant 1 ... signals + functions + processes ... for circuits like majority_voter
            // variant 2 ... functions only ... no signal declarations
            // variant 3 ... signals only ... no functions ... ripple carry adder etc
            // variant 4 ... neither ... simple circuits like and_gate
            $$ = ast_new_node(NODE_ARCH);
            $$->data.arch.name = strdup($2);
            $$->data.arch.entity_name = strdup($4);
            for(int i = 0; i < temp_process_count; i++)
                $$->data.arch.processes[i] = temp_processes[i];
            $$->data.arch.process_count = temp_process_count;
            temp_process_count = 0;
            // CHIRAG 18-04-26 :: copy internal signals into arch node
            for(int i = 0; i < temp_arch_signal_count; i++)
                $$->data.arch.signals[i] = temp_arch_signals[i];
            $$->data.arch.signal_count = temp_arch_signal_count;
            temp_arch_signal_count = 0;
            // CHIRAG 21-04-26 :: copy function declarations into arch node
            for(int i = 0; i < temp_func_count; i++)
                $$->data.arch.funcs[i] = temp_funcs[i];
            $$->data.arch.func_count = temp_func_count;
            temp_func_count = 0;
            printf("parsed architecture: %s with %d processes, %d signals, %d functions\n",
                $2, $$->data.arch.process_count, $$->data.arch.signal_count, $$->data.arch.func_count);
        }
    | ARCHITECTURE IDENTIFIER OF IDENTIFIER IS func_decl_list BEGIN_TOK process_list END_TOK IDENTIFIER ';'
        {
            // CHIRAG 21-04-26 :: functions only ... no signal declarations
            // for circuits that use helper functions but no internal wires
            $$ = ast_new_node(NODE_ARCH);
            $$->data.arch.name = strdup($2);
            $$->data.arch.entity_name = strdup($4);
            for(int i = 0; i < temp_process_count; i++)
                $$->data.arch.processes[i] = temp_processes[i];
            $$->data.arch.process_count = temp_process_count;
            temp_process_count = 0;
            $$->data.arch.signal_count = 0;
            for(int i = 0; i < temp_func_count; i++)
                $$->data.arch.funcs[i] = temp_funcs[i];
            $$->data.arch.func_count = temp_func_count;
            temp_func_count = 0;
            printf("parsed architecture: %s with %d processes, %d functions\n",
                $2, $$->data.arch.process_count, $$->data.arch.func_count);
        }
    | ARCHITECTURE IDENTIFIER OF IDENTIFIER IS signal_decl_list BEGIN_TOK process_list END_TOK IDENTIFIER ';'
        {
            // CHIRAG 18-04-26 :: signals only ... no functions
            // circuits like and_gate have no signal declarations ... this keeps them working
            // CHIRAG 21-04-26 :: kept this variant ... func_count set to 0 explicitly
            $$ = ast_new_node(NODE_ARCH);
            $$->data.arch.name = strdup($2);
            $$->data.arch.entity_name = strdup($4);
            for(int i = 0; i < temp_process_count; i++)
                $$->data.arch.processes[i] = temp_processes[i];
            $$->data.arch.process_count = temp_process_count;
            temp_process_count = 0;
            // CHIRAG 18-04-26 :: copy internal signals into arch node
            for(int i = 0; i < temp_arch_signal_count; i++)
                $$->data.arch.signals[i] = temp_arch_signals[i];
            $$->data.arch.signal_count = temp_arch_signal_count;
            temp_arch_signal_count = 0;
            $$->data.arch.func_count = 0;
            printf("parsed architecture: %s of %s with %d processes, %d internal signals\n",
                $2, $4, $$->data.arch.process_count, $$->data.arch.signal_count);
        }
    | ARCHITECTURE IDENTIFIER OF IDENTIFIER IS BEGIN_TOK process_list END_TOK IDENTIFIER ';'
        {
            // CHIRAG 18-04-26 :: no internal signals ... original rule still works
            // circuits like and_gate have no signal declarations ... this keeps them working
            // CHIRAG 21-04-26 :: added func_count = 0 explicitly ... walker needs this
            $$ = ast_new_node(NODE_ARCH);
            $$->data.arch.name = strdup($2);
            $$->data.arch.entity_name = strdup($4);
            for(int i = 0; i < temp_process_count; i++)
                $$->data.arch.processes[i] = temp_processes[i];
            $$->data.arch.process_count = temp_process_count;
            temp_process_count = 0;
            $$->data.arch.signal_count = 0;
            $$->data.arch.func_count = 0;
            printf("parsed architecture: %s of %s with %d processes\n",
                $2, $4, $$->data.arch.process_count);
        }
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
            // CHIRAG 21-04-26 :: this part is for when no after delay ... same as before
            // delay_ns = 0 means schedule at delta+1 of current time
            $$ = ast_new_node(NODE_ASSIGN);
            $$->data.assign.target = strdup($1);
            $$->data.assign.expr = $3;
            $$->data.assign.delay_ns = 0;
            printf("parsed assignment: %s <=\n", $1);
        }
    | IDENTIFIER ASSIGN expression AFTER_TOK INTEGER NS_TOK ';'
        {
            // CHIRAG 21-04-26 :: this part is for when after delay is used ... Y <= A and B after 10 ns;
            // delay_ns stores the delay value ... ast_walker uses it to schedule
            // at current_time + delay_ns instead of current_time delta+1
            $$ = ast_new_node(NODE_ASSIGN);
            $$->data.assign.target = strdup($1);
            $$->data.assign.expr = $3;
            $$->data.assign.delay_ns = $5;
            printf("parsed assignment with delay: %s <= after %d ns\n", $1, $5);
        }
    ;
    
if_statement
    : IF IDENTIFIER '=' bit_literal THEN { $<num>$ = temp_stmt_count; temp_stmt_count = 0; } statement_list END_TOK IF ';'
        {
            // CHIRAG 18-04-26 :: fixed if statement parsing
            // old code never stored inner statements into NODE_IF
            // they leaked into outer process statement list instead
            // fix ... save temp_stmt_count before parsing inner statements
            // then copy them into if node and restore count
            //
            // CHIRAG 22-04-26 :: extended to support nested ifs
            // old rule had no mid-rule action ... temp_stmts was shared flat array
            // problem ... if T='1' then Q<=not Q; end if; inside outer if
            // inner if statements went into temp_stmts ... outer if grabbed all of them
            // thinking they were its own statements ... nested if completely broken
            //
            // fix ... mid-rule action at $6 saves outer count and resets to 0
            // so inner parsing starts fresh ... outer restores count after
            // same trick already used for else branch ... just applied to then-block too
            int saved = $<num>6;
            $$ = ast_new_node(NODE_IF);
            $$->data.if_stmt.signal_name = strdup($2);
            $$->data.if_stmt.bit_value = $4;
            // copy inner statements that were collected into temp_stmts
            // these belong to the if block not the outer process
            $$->data.if_stmt.statement_count = temp_stmt_count;
            for(int i = 0; i < temp_stmt_count; i++)
                $$->data.if_stmt.statements[i] = temp_stmts[i];
            $$->data.if_stmt.else_statement_count = 0;
            // restore outer count ... outer process picks up from where it left off
            temp_stmt_count = saved;
            printf("parsed if: %s = '%d' with %d statements\n", $2, $4, $$->data.if_stmt.statement_count);
        }
    | IF IDENTIFIER '=' bit_literal THEN { $<num>$ = temp_stmt_count; temp_stmt_count = 0; } statement_list { $<num>$ = temp_stmt_count; } ELSE_TOK { temp_stmt_count = 0; } statement_list END_TOK IF ';'
        {
            // CHIRAG 18-04-26 :: if with else branch
            // problem here ... both then-block and else-block use temp_stmts
            // by the time we reach this action ... temp_stmts has ONLY the else statements
            // why? ... then-block statements were already consumed by first statement_list
            // and temp_stmt_count was reset by ... wait no it wasnt reset between the two lists
            // so temp_stmts has ALL statements ... then + else concatenated
            // we cant split them here cleanly without knowing where then ends
            //
            // fix ... use a separator ... save then-count before parsing else
            // but bison actions run AFTER the whole rule matches ... too late
            //
            // real fix ... use a mid-rule action to snapshot temp_stmt_count
            // after first statement_list but before ELSE_TOK
            // $<num>5 trick ... store count in a typed mid-rule slot
            //
            // CHIRAG 22-04-26 :: extended same fix to then-block too
            // now both then and else blocks save/restore around nested ifs
            // saved_before restores outer context after whole if-else done
            int saved_before = $<num>6;
            int then_count = $<num>8;
            int else_count = temp_stmt_count;
            $$ = ast_new_node(NODE_IF);
            $$->data.if_stmt.signal_name = strdup($2);
            $$->data.if_stmt.bit_value = $4;
            // then-block count was saved by mid-rule action
            // else-block count is whatever was collected after reset
            $$->data.if_stmt.statement_count = then_count;
            for(int i = 0; i < then_count; i++)
                $$->data.if_stmt.statements[i] = temp_stmts[i];
            $$->data.if_stmt.else_statement_count = else_count;
            for(int i = 0; i < else_count; i++)
                $$->data.if_stmt.else_statements[i] = temp_stmts[i];
            // restore outer context
            temp_stmt_count = saved_before;
            printf("parsed if-else: %s = '%d' then=%d else=%d\n", $2, $4, then_count, else_count);
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
    | expression XOR_TOK expression
        {
            $$ = ast_new_node(NODE_EXPR);
            $$->data.expr.expr_type = EXPR_XOR;
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
    | IDENTIFIER '(' arg_list ')'
        {
            // CHIRAG 21-04-26 :: function call ... majority(A, B, C)
            // IDENTIFIER followed by ( means its a function call not a signal read
            // bison lookahead handles the ambiguity ... sees ( after IDENTIFIER = call
            $$ = ast_new_node(NODE_FUNC_CALL);
            $$->data.func_call.name = strdup($1);
            $$->data.func_call.arg_count = temp_arg_count;
            for(int i = 0; i < temp_arg_count; i++)
                $$->data.func_call.args[i] = temp_args[i];
            temp_arg_count = 0;
            printf("parsed func call: %s with %d args\n", $1, $$->data.func_call.arg_count);
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
    if(result != 0) 
    { 
        printf("parsing failed\n"); 
        return 1; 
    }
    
    printf("parsing done! walking AST now...\n");

    DynArray_Signal signals;
    DYNARRAY_INIT(signals)
    Scheduler sch = scheduler_init();

    // walk entity first ... creates all signals from port declarations
    // walk arch second ... creates all processes and adds them to scheduler
    ast_walk(ast_entity, &signals, &sch);
    ast_walk(ast_root, &signals, &sch);
    printf("AST walk done! %d signals created\n", signals.size);

    // CHIRAG 04-04-26 :: build dependency graph after ast_walk
    // process count comes from scheduler ... ast_walk populates it
    // dependency_extract walks AST finds read/write sets per process
    // builds edges where processes share signals ... then colors graph
    // same color = no conflicts = can run in parallel
    DepGraph* g = graph_build(sch.process_ARRAY.size);
    dependency_extract(ast_root, g);
    // CHIRAG 20-04-26 :: write DOT file right after graph is built and colored
    graph_write_dot(g, "dependency.dot");
    // graph_free(g);

    EventQueue eq = init_queue();

    // walker_queue = &eq;
    // CHIRAG 15-04-26 :: thread 0 gets the main queue ... sequential mode uses thread 0
    // omp_get_thread_num() returns 0 in sequential ... so run_proc_generic picks up correct queue
    walker_queues[0] = &eq;
    // CHIRAG 13-04-26 :: collect all input signals for testbench generation
    // direction 0 = input ... set in ast_walker when walking NODE_PORT
    // int input_signals[64];
    int input_signals[256];
    int input_count = 0;
    for(int i = 0; i < signals.size; i++)
        if(signals.data[i].direction == 0)
            input_signals[input_count++] = i;

    Event e;
    e.type = 0; e.delta = 0;

    // CHIRAG 13-04-26 :: two modes for seeding events
    // mode 1 ... manual testbench file passed as argv[2]
    // format is simple ... SIGNAL_NAME VALUE TIME ... one per line ... # for comments
    // mode 2 ... auto generate input combinations ... capped at 16 so output stays readable
   // if(argc > 2)
   // CHIRAG 20-04-26 :: check argv[2] is actually a testbench not a mode flag
    // when no TB given ... makefile passes --seq as argv[2] ... old check opened it as file
    // fix ... if argv[2] starts with "--" its a mode flag not a testbench ... skip to auto mode
    if(argc > 2 && argv[2][0] != '-')
    {
        // manual testbench mode ... user controls exactly what gets tested
        // good for sequential circuits ... clocks ... specific scenarios
        printf("testbench mode: reading from %s\n", argv[2]);
        FILE* tb = fopen(argv[2], "r");
        if(tb == NULL)
        {
            printf("error: cant open testbench %s\n", argv[2]);
            return 1;
        }

        char sig[64]; int val; double time;
        char line[128];
        while(fgets(line, 128, tb))
        {
            // skip comments and empty lines
            if(line[0] == '#' || line[0] == '\n') continue;
            if(sscanf(line, "%s %d %lf", sig, &val, &time) == 3)
            {
                e.signal_name = strdup(sig);
                e.new_value = val;
                e.time = time;
                e.delta = 0;
                e.type = 0;
                insert_ele(&eq, e);
                printf("tb: %s = %d at t=%.0f\n", sig, val, time);
            }
        }
        fclose(tb);
    }
    else
    {
        // auto testbench mode ... generates all 2^N input combinations
        // capped at 16 combinations so output stays readable for large circuits
        // good for combinational circuits ... exhaustive for small input counts
        int limit = (1 << input_count);
        if(limit > 16) limit = 16;
        printf("auto testbench: found %d input signals ... generating %d combinations\n",
            input_count, limit);

        // combination c ... bit j of c = value of input signal j
        // so c=0 means all inputs 0 ... c=1 means first input 1 rest 0 ... etc
        for(int c = 0; c < limit; c++)
            for(int j = 0; j < input_count; j++)
            {
                e.signal_name = signals.data[input_signals[j]].name;
                e.new_value = (c >> j) & 1;
                e.time = c + 1;
                insert_ele(&eq, e);
            }
    }

    // CHIRAG 13-04-26 :: vcd filename from argv[1] ... default to output-parser.vcd
    char vcd_name[64];
    if(argc > 1)
        snprintf(vcd_name, 64, "output-%s.vcd", argv[1]);
    else
        snprintf(vcd_name, 64, "output-parser.vcd");

    vcd_init(vcd_name);
    vcd_write_header(&signals);
    init_run();
    trace_init();
    
    // CHIRAG 17-04-26 :: mode flag ... argv[3] is --seq or --par
    
    // old method was that i had both runs and was commenting a functtion call then running other
    // well that has been annowing so will be adding a mode variable to ask for the mode and based on that we run mode
    
    // run_simulation(&eq, &sch, &signals);
    // run_parallel_simulation(&eq, &sch, &signals, g);

    
    // default is parallel if not specified
    // just now it occured to me... like why not clock?
    // ddoesnt it ddo the same thing? isnt it same as Openmp?
    // timing uses omp_get_wtime() not clock()
    // clock() measures total CPU time across all threads ... if 4 threads run 1s each clock() = 4s
    // omp_get_wtime() measures wall clock time ... actual real world time user waited
    // speedup = seq_wall_time / par_wall_time ... this is why we never use clock to test speedup
    int use_parallel = 1;
    // if(argc > 3 && strcmp(argv[3], "--seq") == 0)
    //     use_parallel = 0;
    // NEW -- CHIRAG 21-04-26 :: fix mode detection when no TB file given
    // old code assumed argv[3] always has mode flag ... only true when TB is present
    // when no TB ... argv[2] is the mode flag ... argv[3] doesnt exist
    // fix ... check both argv[2] and argv[3] for --seq flag
    // if((argc > 2 && strcmp(argv[2], "--seq") == 0) || (argc > 3 && strcmp(argv[3], "--seq") == 0))
    //   use_parallel = 0;
    // CHIRAG 22-04-26 :: parse --seq and --stress N from argv
    // problem ... old code only checked argv[2] and argv[3] for --seq
    // now we have --stress N as a third optional flag ... old approach breaks
    // fix ... loop all argv and handle each flag by name ... order doesnt matter
    // --stress N sets stress_ns in ast_walker.c via extern
    // --seq sets use_parallel=0 ... everything else is ignored
    // extern int stress_ns;
    // for(int i = 1; i < argc; i++)
    // {
    //     if(strcmp(argv[i], "--seq") == 0)
    //         use_parallel = 0;
    //     else if(strcmp(argv[i], "--stress") == 0 && i + 1 < argc)
    //     {
    //         stress_ns = atoi(argv[i + 1]);
    //         i++; // skip the number ... already consumed
    //     }
    // }
    // printf("stress: %d ns per process\n", stress_ns);

    // CHIRAG 22-04-26 :: parse --seq and --stress N from argv
    // loop all argv ... order doesnt matter ... --stress 1000 --seq same as --seq --stress 1000
    // extern stress_ns defined in ast_walker.c ... set here before sim runs
    extern int stress_ns;
    for(int i = 1; i < argc; i++)
    {
        if(strcmp(argv[i], "--seq") == 0)
            use_parallel = 0;
        else if(strcmp(argv[i], "--stress") == 0 && i + 1 < argc)
        {
            stress_ns = atoi(argv[i + 1]);
            i++;
        }
    }
    printf("stress: %d flops per gate\n", stress_ns);

    double t_start = omp_get_wtime();
    if(use_parallel)
        run_parallel_simulation(&eq, &sch, &signals, g);
    else
        run_simulation(&eq, &sch, &signals);
    double t_end = omp_get_wtime();
    printf("mode: %s\n", use_parallel ? "parallel" : "sequential");
    printf("threads: %d\n", omp_get_max_threads());
    printf("simulation time: %.9f seconds\n", t_end - t_start);
    printf("\nfinal signal values:\n");
    for(int i = 0; i < signals.size; i++)
        printf("  %s = %d\n", signals.data[i].name, signals.data[i].value);

    // CHIRAG 13-04-26 :: print hash at end ... this is the correctness contract
    // sequential hash must equal parallel hash later ... if they differ there is a bug
    printf("trace hash: %u\n", trace_hash());
    // CHIRAG 20-04-26 :: print simulation stats
    // idea ... give user insight into what the sim actually did internally
    // problem ... before this ... black box ... hash and values but no internal metrics
    // solution ... four counters updated throughout sim ... printed here at end
    // delta_count tells how many delta cycles ran ... feedback chain depth story
    // event_count tells how many external stimulus events were processed
    // process_firings tells how many times processes actually executed
    // max_delta_depth is the most interesting ... deepest feedback chain seen
    printf("\n----> simulation stats <----\n");
    printf("total delta cycles    : %d\n", stat_delta_count);
    printf("total events processed: %d\n", stat_event_count);
    printf("total process firings : %d\n", stat_process_firings);
    printf("max delta depth       : %d\n", stat_max_delta_depth);
    printf("--------------------------\n");
    // CHIRAG 21-04-26 :: Amdahl's law theoretical speedup
    // idea ... show what speedup is theoretically possible given our parallel fraction
    // problem ... measured times on small circuits are noisy ... timer resolution too coarse
    // solution ... compute theoretical Amdahl numbers from graph structure
    // serial fraction S = 1 / num_colors ... one color batch must run serially before next
    // if all processes same color ... S is just overhead fraction ... estimate 0.3
    // Amdahl ... Speedup(N) = 1 / (S + (1-S)/N)
    // this gives upper bound on speedup ... real speedup will be lower due to overhead
    // float S = (g->num_colors > 1) ? (1.0f / g->num_colors) : 0.3f;
    // printf("\n--- amdahl's law (theoretical) ---\n");
    // printf("serial fraction S     : %.2f\n", S);
    // printf("parallel fraction     : %.2f\n", 1.0f - S);
    // for(int n = 1; n <= 16; n *= 2)
    // {
    //     float speedup = 1.0f / (S + (1.0f - S) / n);
    //     printf("threads=%2d  speedup   : %.2fx\n", n, speedup);
    // }
    // printf("----------------------------------\n");
    graph_free(g); 
    vcd_close();
    return 0;
}