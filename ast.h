#ifndef AST_H
#define AST_H


#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "lexVal.h"
#include "iloc.h"

typedef struct node{
	struct lexval* token;
	Type varType;
	int kidsNumber;
	struct node **kids; // lista de ponteiros

  InstructionList* instructions;

} Node;

void checkTree(void* voidNode);

int removeNullNode(Node* node);

Node* removeNullHead(Node* node);

void printTree(Node* node);

void printTokenLabel(Node* node);

Node* createNode(struct lexval* token, int tokenType);

Node* createDanglingNode(struct lexval* token);

void addChild(Node *pai, Node *kid);

void addType(Node *node, Type type);

void addTypeRec(Node *node, Type type);

void addNature(Node *node, Nature nature);

void printToken(Node* node);

void exporta(void *voidNode);

void libera(void *voidNode);

void freeDanglingScanner(Node *n);

void freeDanglingParser(Node *n);

void nullifyPointer(struct lexval* token, Node *n);

void contaNodosNaRaiz(Node *n);

void changeTokenType(Node* node);

#endif
