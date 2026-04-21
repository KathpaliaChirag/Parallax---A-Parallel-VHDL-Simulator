%{
// CHIRAG 18-03-26 : 22:36 :: okay so here is the lexer for our VHDL subset
// 
// so what even is a lexer.... it breaks raw text input into tokens
// I give it "entity AND_GATE is" and it spits out
// ENTITY, IDENTIFIER("AND_GATE"), IS .... one by one
// flex reads this .l file and generates a C file with yylex() function
// every time parser calls yylex() it gets the next token
//
// VHDL SUBSET WE SUPPORT :
//  for first submission 
// I support just enough to describe basic digital circuits for our simulator
// specifically :
// 1. entity declaration .... defines circuit name and its ports (inputs/outputs)
// 2. architecture .... describes what the circuit actually does
// 3. process with sensitivity list .... wakes up when signals change
// 4. signal assignments with <= operator
// 5. boolean expressions .... and, or, not
// 6. if/then for flip flop clock edge detection
// 7. bit literals '0' and '1'
//
// CIRCUITS WE CAN DESCRIBE :
// AND gate, OR gate, NOT gate .... basic combinational logic
// D flip flop .... sequential logic with clock edge
// T flip flop .... toggle flip flop
//
// WHAT WE SKIP FOR NOW :
// generics, packages, libraries, std_logic, integers, arrays
// multiple architectures, component instantiation, generate statements
// basically anything beyond simple gates and flip flops
//
// FLOW :
// .vhdl file → flex lexer → tokens → bison parser → AST → simulator
// the AST then creates Signal and Process structs automatically
// no more hardcoding circuits in main.c .... parser does it for us



// CHIRAG 18-03-26 : 21:56 :: Okay so here starts lexer
// this will be our lexer file i will start small for demo and add later as we make it bigger
// part 1 : raw C code copied to top of generated file
// we put includes here
// basic rules to write lexer file here goes again lexer.l has this %% which seperates three sections
// patters used are usually regx or quotes string
// the return statements sends token to parser
// if we want token to be consumed and yet make no output we use { /* used like this to skip */ }
// [^\n]* : it means you take all except \n
#define YY_NO_INPUT
#define YY_NO_UNPUT
#include "parser.h"
#include <string.h>
%}

%%

"entity"        { return ENTITY; }
"begin"         { return BEGIN_TOK; }
"architecture"  { return ARCHITECTURE; }
"port"          { return PORT; }
"process"       { return PROCESS; }
"end"           { return END_TOK; }
"is"            { return IS; }
"of"            { return OF; }
"in"            { return IN_TOK; }
"out"           { return OUT_TOK; }
"bit"           { return BIT; }
"and"           { return AND_TOK; }
"or"            { return OR_TOK; }
"xor"           { return XOR_TOK; }
"not"           { return NOT_TOK; }
"if"            { return IF; }
"else"          { return ELSE_TOK; }
"then"          { return THEN; }
"signal"        { return SIGNAL; }
"after"         { return AFTER_TOK; }
"ns"            { return NS_TOK; }
[0-9]+          { yylval.num = atoi(yytext); return INTEGER; }
\'0\'           { yylval.num = 0; return ZERO; }
\'1\'           { yylval.num = 1; return ONE; }
[a-zA-Z_][a-zA-Z0-9_]*  { yylval.str = strdup(yytext); return IDENTIFIER; }
[ \t\n]+        { /* skip whitespace */ }
"--"[^\n]*      { /* skip comment */ }
"<="            { return ASSIGN; }
":"             { return ':'; }
";"             { return ';'; }
","             { return ','; }
"("             { return '('; }
")"             { return ')'; }
"="             { return '='; }
"'"             { return '\''; }
.               { printf("Unknown character: %s\n", yytext); }

%%
// section 3: user C functions (optional)

int yywrap() { return 1; }