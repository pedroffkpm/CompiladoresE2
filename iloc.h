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
  ADD,
  ADDI,
  SUB,
  MULT,
  DIV,
  LOAD,
  LOADI,
  LOADA0,
  LOADAI,
  CLOAD,
  CLOADAI,
  CLOADA0,
  RSUBI,
  STORE,
  STOREAI,
  STOREA0,
  CSTORE,
  CSTOREAI,
  CSTOREA0,
  I2I,
  C2C,
  C2I,
  I2C,
  CMP_EQ,
  CMP_LE,
  CMP_LT,
  CMP_GE,
  CMP_GT,
  CMP_NE,
  CBR,
  JUMPI,
  JUMP,
  HALT
} OpCode;

typedef struct Instruction
{
  OpCode op_code; //sabe pelo code se arg é reg, label, constante, etc
  int label;
  int arg1;
  int arg2;
  int arg3;

} Instruction;

typedef struct InstList {
  Instruction** instructions;
  int inst_num;

} InstructionList;

int getRegister();

int getLabel();

InstructionList* createList();

InstructionList* concatLists( InstructionList* list1, InstructionList* list2);

void addInstToList(Instruction* inst, InstructionList* list);

Instruction* createInstruction(OpCode op, int arg1, int arg2, int arg3);


Instruction* add(int op1, int op2, int dest);
Instruction* sub(int op1, int op2, int dest);
Instruction* mult(int op1, int op2, int dest);
Instruction* divI(int op1, int op2, int dest);


Instruction* addI(int op1, int op2, int dst);
Instruction* rsubI(int reg, int op, int dst);

Instruction* loadI(int value, int reg);
Instruction* loadAI(int src, int offset, int dst);
Instruction* storeAI(int src, int dst, int offset);
Instruction* i2i(int r1, int r2);

Instruction* jumpI(int label);
Instruction* jump(int reg);

Instruction* halt();