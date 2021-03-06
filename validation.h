#ifndef VALIDATION_H
#define VALIDATION_H 
#include "table.h"
#include "errors.h"

void validateProgram(void* arvore);
void validateTable(SymbolTable* table, Node* node);
void validateInput(Node* node);
void validateOutput(Node* node);
void validateReturn(Node* node);
void validateAttribution(Node *node);
idList* createId(struct lexval *token1, struct lexval *token2, struct idList* next);
int addIdsToTable(idList* ids, Type type);

void printSimpleError(int errorcode, int lineNumber, char* name);

void addFuncToTable(struct lexval* token, Type type, Param* param);

void addNextId(struct idList* current,struct idList* next);

void validateFunction(Node* node);

void validateShift(Node *node);

void addNodeToTable(Node* node, Type type, Nature nature, int size);

#endif
