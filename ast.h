#ifndef AST_H
#define AST_H 
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "lexVal.h"
#include "instructions.h"

typedef struct node{
	struct lexval* token;
	Type varType;
	int kidsNumber;
  int regTemp; //registrador temporário
  int label;
	struct node **kids; // lista de ponteiros

  int trueNmr;
  int* tl;

  int falseNmr;
  int* fl;

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

void addTrueList(Node* node, int label);

void concatTrueL(Node* node1, Node* node2);

void concatFalseL(Node* node1, Node* node2);

void addFalseList(Node* node, int label);

#endif
