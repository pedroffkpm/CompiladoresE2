#include "ast.h"
#include "table.h"
#include "errors.h"

void validateProgram(void* arvore);
void validateTable(SymbolTable* table, Node* node);
