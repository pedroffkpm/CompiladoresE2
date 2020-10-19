#ifndef VALIDATION_H
#define VALIDATION_H 
#include "table.h"
#include "errors.h"

void validateProgram(void* arvore);
void validateTable(SymbolTable* table, Node* node);
void validateInput(Node* node);
void validateOutput(Node* node);
void validateReturn(Node* node);

#endif
