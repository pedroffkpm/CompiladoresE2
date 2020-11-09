#include <stdlib.h>
#include "ast.h"
#include "table.h"

typedef enum { //negativos para não conflitar com registradores criados com getRegister()
    RBSS = -3,
    RFP,
    RSP
} Register;

typedef enum
{
  LBL = -1, // Lr1: 
  NOP, // nop 
  ADD, // r3 = r1 + r2 | registradores
  ADDI, // r3 = r1 + c2 (constante/offset)
  SUB, // r3 = r1 - r2
  SUBI, // r3 = r1 - c2 | constante/offset || acho que nunca é usado por nós
  RSUBI, // r3 = c2 - r1
  MULT, // r3 = r1 * r2
  MULTI, // r3 = r1 * c2 |constante || nunca é usado
  DIV, // r3 = r1 / r2
  DIVI, // r3 = r1 / c2 || acho que nunca é usado
  RDIVI, // r3 = c2 / r1 || acho que nunca é usado
  // LOAD, // r2 = Mem(r1) | EQUIVALE A LOADAI PASSANDO 0
  LOADI, // r2 = c1 | constante
  LOADA0, // r3 = Mem(r1 + r2) | registradores || acho que nunca é usado
  LOADAI, // r3 = Mem(r1 + c2) | constante/offset
  // STORE, // Mem(r2) = r1 | STORE É EQUIVALENTE A STOREAI PASSANDO 0, NÃO PRECISA
  STOREAI, // Mem(r2 + c3) = r1
  STOREA0, // Mem(r2 + r3) = r1 
  I2I, // r2 = r1 | int
  JUMPI, // pc = endereco(l1) | label
  JUMP, // pc = r1 | registrador
  CBR, // r1 ? pc = endereco(l2) : pc=endereco(l3)
  CMP_LT, // r3 = (r1 < r2)
  CMP_LE, // r3 = (r1 <= r2)
  CMP_EQ, // r3 = (r1 == r2)
  CMP_GE, // r3 = (r1 >= r2)
  CMP_GT, // r3 = (r1 > r2)
  CMP_NE, // r3 = (r1 != r2)

  HALT //acaba programa
} OpCode;

typedef struct Instruction
{
  OpCode op_code; //sabe pelo code se arg é reg, label, constante, etc
  int arg1;
  int arg2;
  int arg3;

} Instruction;

typedef struct InstList {
  Instruction** instructions;
  int inst_num;

} InstructionList;

//####

int getRegister();

int getLabel();

InstructionList* createList();

InstructionList* concatLists( InstructionList* list1, InstructionList* list2);

void addInstToList(Instruction* inst, InstructionList* list);

Instruction* createInstruction(OpCode op, int arg1, int arg2, int arg3);

//#####

//instructions
Instruction* addI(int reg, int op, int dst);

Instruction* loadI(int c, int reg);
Instruction* loadAI(int src, int offset, int dst);
Instruction* storeAI(int src, int dst, int offset);
Instruction* i2i(int r1, int r2);

Instruction* jumpI(int label);
Instruction* jump(int reg);

Instruction* cbr(int r, int label_true, int label_false);

Instruction* halt();

//code

void loadVarToRegCode(Node* node);

void intCode(Node* node);

void binOpCode(Node* node);

void arithmeticCode(Node* node);

void booleanCode(Node* node);

void logicCode(Node* node);

void remendaTrue(Node* node, int newLabel);

void remendaFalse(Node* node, int newLabel);

void relopCode(Node* node, OpCode op);

void assignCode(Node* node);
