// CHIRAG 18-03-26 23:15 :: okay so AST.... Abstract Syntax Tree
// so the parser reads tokens and builds this tree in memory
// think of it like a structured representation of the circuit
// instead of just printing "parsed entity" we actually store the data
// later the AST walker walks this tree and creates Signal + Process structs
// 
// in C to make a node that can be different types i use a union inside a struct
// the type field tells me which union member is active
// classic C pattern for polymorphism without C++

#ifndef AST_H
#define AST_H
#include <stdlib.h>
#include <string.h>

// CHIRAG : all possible node types in my AST
typedef enum {
    NODE_ENTITY,      // entity AND_GATE is port(...); end AND_GATE;
    NODE_PORT,        // A : in bit
    NODE_ARCH,        // architecture RTL of AND_GATE is ... end RTL;
    NODE_PROCESS,     // process(A,B) begin ... end process;
    NODE_ASSIGN,      // Y <= A and B;
    NODE_IF,          // if CLK = '1' then ... end if;
    NODE_EXPR,        // expressions.... AND OR NOT IDENTIFIER
} NodeType;

typedef enum {
    DIR_IN,
    DIR_OUT,
    DIR_INTERNAL   // CHIRAG 18-04-26 :: internal wire ... signal X : bit; in architecture
} PortDir;

// CHIRAG : expression type.... what kind of expression is this node
// CHIRAG 18-04-26 :: added EXPR_XOR ... sits right next to EXPR_AND and EXPR_OR
typedef enum {
    EXPR_AND,
    EXPR_OR,
    EXPR_XOR,    // new
    EXPR_NOT,
    EXPR_IDENTIFIER,
    EXPR_BIT_LITERAL
} ExprType;
 
// forward declaration.... ASTNode refers to itself so i need this
typedef struct ASTNode ASTNode;

// CHIRAG : dynamic array of ASTNode pointers.... for lists of ports, processes, statements
// using a simple fixed array for now.... can make dynamic later if needed
#define MAX_CHILDREN 32

// CHIRAG : the main AST node struct
// type field tells me which union member to look at
// this is the classic C tagged union pattern
struct ASTNode {
    NodeType type;  // what kind of node is this

    union {
        // entity node.... stores name and list of port nodes
        struct {
            char* name;
            ASTNode* ports[MAX_CHILDREN];
            int port_count;
        } entity;

        // port node.... one port declaration
        struct {
            char* name;
            PortDir direction;
        } port;

        // architecture node.... stores name and list of process nodes
        struct {
            char* name;
            char* entity_name;
            ASTNode* processes[MAX_CHILDREN];
            int process_count;
            // CHIRAG 18-04-26 :: added internal signals array
            // ports live in entity ... internal wires live here in arch
            // signal CARRY : bit; between IS and BEGIN goes here
            ASTNode* signals[MAX_CHILDREN];
            int signal_count;
        } arch;
        // process node.... sensitivity list and statements
        struct {
            char* sensitivity[MAX_CHILDREN];  // signal names
            int sensitivity_count;
            ASTNode* statements[MAX_CHILDREN];
            int statement_count;
        } process;

        // assignment node.... Y <= expression
        struct {
            char* target;       // left side signal name
            ASTNode* expr;      // right side expression
        } assign;

        // if node.... if CLK = '1' then ... end if
        struct {
            char* signal_name;
            int bit_value;
            ASTNode* statements[MAX_CHILDREN];
            int statement_count;
            // CHIRAG 18-04-26 :: added else branch
            // if CLK='1' then ... else ... end if
            // else_statements is empty if no else branch present
            ASTNode* else_statements[MAX_CHILDREN];
            int else_statement_count;
        } if_stmt;

        // expression node.... AND OR NOT or just an identifier
        struct {
            ExprType expr_type;
            char* identifier;   // used when expr_type == EXPR_IDENTIFIER
            int bit_value;      // used when expr_type == EXPR_BIT_LITERAL
            ASTNode* left;      // left child
            ASTNode* right;     // right child (NULL for NOT and IDENTIFIER)
        } expr;

    } data;
};

// CHIRAG : helper function to create a new AST node
// malloc a node, set its type, return pointer
ASTNode* ast_new_node(NodeType type);

// CHIRAG : print the AST for debugging.... walks tree and prints structure
void ast_print(ASTNode* node, int depth);

// CHIRAG : free all memory used by AST
void ast_free(ASTNode* node);

#endif