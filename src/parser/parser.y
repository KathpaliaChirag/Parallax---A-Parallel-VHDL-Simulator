%{
// CHIRAG 18-03-26 23:39 :: okay so this is parser.y .... the grammar file
// bison reads this and generates parser.c and parser.h
// parser.h has all token numbers which lexer.l includes
// parser.c has yyparse() function which i call from main to parse a vhdl file
//
// how bison grammar works :
// i write rules like "an entity is ENTITY IDENTIFIER IS PORT ..."
// bison matches tokens from lexer against these rules
// when a rule matches i can run C code in the { } action block
// that C code builds my AST
//
// $$ $1 $2 etc are how i pass values up the parse tree
// $$ = value of current rule
// $1 = value of first token/rule in pattern, $2 = second etc

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include "ast.h"

// ASTNode* ast_root = NULL;  // will hold the final parsed tree

void yyerror(const char* s);
int yylex();
extern char* yytext;  // current token text from lexer
struct ASTNode;
extern struct ASTNode* ast_root;
// forward declare so yyparse can use it
// actual definition is in ast.c
%}

%code requires {
    #include "ast.h"
}

// CHIRAG : union must come first.... tells bison what types values can have
%union {
    char* str;      // for identifiers
    int num;        // for bit literals
    ASTNode* node;  // for AST nodes
}

// CHIRAG : token declarations with types
// <str> means this token carries a char* value
// <num> means this token carries an int value
%token <str> IDENTIFIER
%token <num> ZERO ONE
%token ENTITY ARCHITECTURE PORT PROCESS
%token BEGIN_TOK END_TOK IS OF
%token IN_TOK OUT_TOK BIT
%token AND_TOK OR_TOK NOT_TOK
%token IF THEN SIGNAL
%token ASSIGN

// CHIRAG : type declarations for rules
// tells bison what type each grammar rule returns via $$
%type <node> program entity_decl architecture_decl
%type <node> process_decl statement signal_assignment if_statement
%type <node> expression port_item port_list process_list statement_list
%type <str>  identifier_list
%type <num>  bit_literal

// CHIRAG : operator precedence .... lower listed = lower precedence
// so A and B or C correctly parses as (A and B) or C
%left OR_TOK
%left AND_TOK
%right NOT_TOK

%%

// CHIRAG : top level rule.... a vhdl file has one entity and one architecture
program
    : entity_decl architecture_decl
        {
            // store entity in arch and set as root
            $$ = $2;
            ast_root = $$;
            ast_print(ast_root, 0);
        }
    ;

// CHIRAG : entity declaration
// entity AND_GATE is port(A : in bit; B : in bit; Y : out bit); end AND_GATE;
entity_decl
    : ENTITY IDENTIFIER IS PORT '(' port_list ')' ';' END_TOK IDENTIFIER ';'
        {
            $$ = ast_new_node(NODE_ENTITY);
            $$->data.entity.name = strdup($2);
            // port_list is stored globally for now.... will wire up properly
            printf("parsed entity: %s\n", $2);
        }
    ;

// CHIRAG : port list.... one or more port items separated by ;
// returns the last port item for now.... full list handled via global
port_list
    : port_item
        { $$ = $1; }
    | port_list ';' port_item
        { $$ = $3; }
    ;

// CHIRAG : one port item.... A : in bit or Y : out bit
port_item
    : identifier_list ':' IN_TOK BIT
        {
            $$ = ast_new_node(NODE_PORT);
            $$->data.port.name = strdup($1);
            $$->data.port.direction = DIR_IN;
            printf("parsed input port: %s\n", $1);
        }
    | identifier_list ':' OUT_TOK BIT
        {
            $$ = ast_new_node(NODE_PORT);
            $$->data.port.name = strdup($1);
            $$->data.port.direction = DIR_OUT;
            printf("parsed output port: %s\n", $1);
        }
    ;

// CHIRAG : identifier list.... A or A, B, C
// returns just the first identifier for now
identifier_list
    : IDENTIFIER
        { $$ = $1; }
    | identifier_list ',' IDENTIFIER
        { $$ = $1; }
    ;

// CHIRAG : architecture declaration
// architecture RTL of AND_GATE is begin ... end RTL;
architecture_decl
    : ARCHITECTURE IDENTIFIER OF IDENTIFIER IS BEGIN_TOK process_list END_TOK IDENTIFIER ';'
        {
            $$ = ast_new_node(NODE_ARCH);
            $$->data.arch.name = strdup($2);
            $$->data.arch.entity_name = strdup($4);
            printf("parsed architecture: %s of %s\n", $2, $4);
        }
    ;

// CHIRAG : one or more processes
process_list
    : process_decl
        { $$ = $1; }
    | process_list process_decl
        { $$ = $2; }
    ;

// CHIRAG : process declaration
// process(A, B) begin ... end process;
process_decl
    : PROCESS '(' identifier_list ')' BEGIN_TOK statement_list END_TOK PROCESS ';'
        {
            $$ = ast_new_node(NODE_PROCESS);
            $$->data.process.sensitivity[0] = strdup($3);
            $$->data.process.sensitivity_count = 1;
            printf("parsed process with sensitivity: %s\n", $3);
        }
    ;

// CHIRAG : list of statements
statement_list
    : statement
        { $$ = $1; }
    | statement_list statement
        { $$ = $2; }
    ;

// CHIRAG : statement is assignment or if
statement
    : signal_assignment
        { $$ = $1; }
    | if_statement
        { $$ = $1; }
    ;

// CHIRAG : signal assignment.... Y <= A and B;
signal_assignment
    : IDENTIFIER ASSIGN expression ';'
        {
            $$ = ast_new_node(NODE_ASSIGN);
            $$->data.assign.target = strdup($1);
            $$->data.assign.expr = $3;
            printf("parsed assignment: %s <=\n", $1);
        }
    ;

// CHIRAG : if statement for flip flop clock edge
// if CLK = '1' then ... end if;
if_statement
    : IF IDENTIFIER '=' bit_literal THEN statement_list END_TOK IF ';'
        {
            $$ = ast_new_node(NODE_IF);
            $$->data.if_stmt.signal_name = strdup($2);
            $$->data.if_stmt.bit_value = $4;
            printf("parsed if: %s = '%d'\n", $2, $4);
        }
    ;

// CHIRAG : bit literal '0' or '1'
bit_literal
    : ZERO  { $$ = 0; }
    | ONE   { $$ = 1; }
    ;

// CHIRAG : expressions.... AND OR NOT IDENTIFIER
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

// CHIRAG : bison calls this on syntax error
void yyerror(const char* s)
{
    fprintf(stderr, "parse error: %s\n", s);
}
int main() {
    return yyparse();
}