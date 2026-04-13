#ifndef DEPENDENCY_H
#define DEPENDENCY_H
#include "graph.h"
#include "../parser/ast.h"
void dependecy_extract(ASTNode* root, DepGraph* g);
#endif