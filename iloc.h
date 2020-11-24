#ifndef ILOC_H
#define ILOC_H

#include <stdlib.h>
#include "instructions.h"
#include "ast.h"
#include "table.h"

//code
void loadVarToRegCode(Node* node);

void intCode(Node* node);

void binOpCode(Node* node);

void ternOpCode(Node* node);

void arithmeticCode(Node* node);

void booleanCode(Node* node);

void logicCode(Node* node);

void ifElseCode(Node* node);

void whileCode(Node* node);

void forCode(Node* node);

void functionDeclarationCode(Node* node);

void functionCallCode(Node* node);

void returnCode(Node* node);

void callArgListCode(Node* node);

void blockCode(Node* node);

void relopCode(Node* node, OpCode op);

void assignCode(Node* node);

void iloc_init(Node* node);

void varLocalCode(Node* node);

void componenteCode(Node* pai, Node* filho);

//print

void remendaTrue(Node* node, int newLabel);

void remendaFalse(Node* node, int newLabel);

void remendaReturn(IList* aux);

// void concatInstructions(Node* dst, Node* src, bool toStart);
// void addInstToNode(Instruction* inst, Node* node, bool toStart);

// void freeInstructions(Node* node);

char* special_register(Register nmr);
void printCode(Node* node);
void printInstruction(Instruction instruction);

#endif
