#ifndef DEPENDENCY_H
#define DEPENDENCY_H
#include "graph.h"
#include "../parser/ast.h"
void dependency_extract(ASTNode* root, DepGraph* g);
#endif