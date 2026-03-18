// CHIRAG 18-03-26 23:30 :: ast.c implementation
// three functions here:
// ast_new_node : malloc a node and set its type.... simple
// ast_print : walk the tree and print it.... useful for debugging to see if parser built it correctly
// ast_free : recursively free all memory.... important to not leak

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

ASTNode* ast_root = NULL;
// CHIRAG : create a new node.... just malloc and set type
// calloc zeros out memory so all pointers start as NULL automatically
ASTNode* ast_new_node(NodeType type)
{
    ASTNode* node = calloc(1, sizeof(ASTNode));
    node->type = type;
    return node;
}

// CHIRAG : helper to print indentation based on depth
static void print_indent(int depth)
{
    for(int i = 0; i < depth; i++)
        printf("  ");
}

// CHIRAG : print the AST recursively.... depth controls indentation
// useful to call after parsing to verify tree looks correct
void ast_print(ASTNode* node, int depth)
{
    if(node == NULL) return;

    print_indent(depth);

    switch(node->type)
    {
        case NODE_ENTITY:
            printf("ENTITY: %s\n", node->data.entity.name);
            for(int i = 0; i < node->data.entity.port_count; i++)
                ast_print(node->data.entity.ports[i], depth + 1);
            break;

        case NODE_PORT:
            printf("PORT: %s direction=%s\n",
                node->data.port.name,
                node->data.port.direction == DIR_IN ? "in" : "out");
            break;

        case NODE_ARCH:
            printf("ARCH: %s of %s\n",
                node->data.arch.name,
                node->data.arch.entity_name);
            for(int i = 0; i < node->data.arch.process_count; i++)
                ast_print(node->data.arch.processes[i], depth + 1);
            break;

        case NODE_PROCESS:
            printf("PROCESS sensitivity=(");
            for(int i = 0; i < node->data.process.sensitivity_count; i++)
            {
                printf("%s", node->data.process.sensitivity[i]);
                if(i < node->data.process.sensitivity_count - 1)
                    printf(", ");
            }
            printf(")\n");
            for(int i = 0; i < node->data.process.statement_count; i++)
                ast_print(node->data.process.statements[i], depth + 1);
            break;

        case NODE_ASSIGN:
            printf("ASSIGN: %s <=\n", node->data.assign.target);
            ast_print(node->data.assign.expr, depth + 1);
            break;

        case NODE_IF:
            printf("IF: %s = '%d'\n",
                node->data.if_stmt.signal_name,
                node->data.if_stmt.bit_value);
            for(int i = 0; i < node->data.if_stmt.statement_count; i++)
                ast_print(node->data.if_stmt.statements[i], depth + 1);
            break;

        case NODE_EXPR:
            switch(node->data.expr.expr_type)
            {
                case EXPR_AND:
                    printf("EXPR: AND\n");
                    ast_print(node->data.expr.left, depth + 1);
                    ast_print(node->data.expr.right, depth + 1);
                    break;
                case EXPR_OR:
                    printf("EXPR: OR\n");
                    ast_print(node->data.expr.left, depth + 1);
                    ast_print(node->data.expr.right, depth + 1);
                    break;
                case EXPR_NOT:
                    printf("EXPR: NOT\n");
                    ast_print(node->data.expr.left, depth + 1);
                    break;
                case EXPR_IDENTIFIER:
                    printf("EXPR: IDENTIFIER(%s)\n", node->data.expr.identifier);
                    break;
                case EXPR_BIT_LITERAL:
                    printf("EXPR: BIT('%d')\n", node->data.expr.bit_value);
                    break;
            }
            break;
    }
}

// CHIRAG : free the AST recursively
// free children first then the node itself.... standard recursive free pattern
void ast_free(ASTNode* node)
{
    if(node == NULL) return;

    switch(node->type)
    {
        case NODE_ENTITY:
            for(int i = 0; i < node->data.entity.port_count; i++)
                ast_free(node->data.entity.ports[i]);
            break;

        case NODE_ARCH:
            for(int i = 0; i < node->data.arch.process_count; i++)
                ast_free(node->data.arch.processes[i]);
            break;

        case NODE_PROCESS:
            for(int i = 0; i < node->data.process.statement_count; i++)
                ast_free(node->data.process.statements[i]);
            break;

        case NODE_ASSIGN:
            ast_free(node->data.assign.expr);
            break;

        case NODE_IF:
            for(int i = 0; i < node->data.if_stmt.statement_count; i++)
                ast_free(node->data.if_stmt.statements[i]);
            break;

        case NODE_EXPR:
            ast_free(node->data.expr.left);
            ast_free(node->data.expr.right);
            break;

        default:
            break;
    }

    free(node);
}