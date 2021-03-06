#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "lexVal.h"

typedef struct node{
	struct lexval* token;
	int kidsNumber;
	struct node **kids; // lista de ponteiros
} Node;

void printTree(Node* node);

void printTokenLabel(Node* node);

Node* createNode(struct lexval* token, int tokenType);

Node* createDanglingNode(struct lexval* token);

void addChild(Node *pai, Node *kid);

void printToken(Node* node);

void exporta(void *voidNode);

void libera(void *voidNode);

void freeDanglingScanner(Node *n);

void freeDanglingParser(Node *n);

void nullifyPointer(struct lexval* token, Node *n);

void contaNodosNaRaiz(Node *n);

