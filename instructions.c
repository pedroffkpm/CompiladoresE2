#include "instructions.h"

int register_counter = 0;
int label_counter = 1; //começa em 1 pq 0 é reservado pra main()

//################

Instruction* lbl(int label) {
  return createInstruction(LBL, label, 0, 0);
}

Instruction* jumpI(int label) {
  return createInstruction(JUMPI, label, 0, 0);
}

Instruction* jump(int reg) {
  return createInstruction(JUMP, reg, 0, 0);
}

Instruction* halt() {
  return createInstruction(HALT, 0, 0, 0);
}

Instruction* addI(int reg, int op2, int dst) { //addI reg, c2 => r3 | dst = reg + op2
    return createInstruction(ADDI, reg, op2, dst);
}

Instruction* loadI(int c, int reg) { //loadI c => reg | reg = c
    return createInstruction(LOADI, c, reg, 0);
}

Instruction* loadAI(int src, int offset, int dst) { //loadAI src, offset => dest | dest = Memoria(src + offset)
    return createInstruction(LOADAI, src, offset, dst);
}

Instruction* storeAI(int src, int dst, int offset) {
    return createInstruction(STOREAI, src, dst, offset);
}

Instruction* i2i(int r1, int r2) {
    return createInstruction(I2I, r1, r2, 0);
}

Instruction* cbr(int r, int label_true, int label_false) {
  return createInstruction(CBR, r, label_true, label_false);
}

int getRegister() {
  register_counter++;
  return register_counter;
}

int getLabel() {
  label_counter++;
  return label_counter;
}

//###############

InstructionList* createList() {
  InstructionList* list = (InstructionList*) malloc((sizeof(InstructionList)));
  list->inst_num = 0;
  list->instructions = NULL;

  return list;
}

void freeList(InstructionList* list) {
  for(int i=0; i<list->inst_num; i++) {
    free(list->instructions[i]);
    list->instructions[i] = NULL;
  }

  free(list);
  list = NULL;

}

InstructionList* concatLists( InstructionList* list1, InstructionList* list2) {
  InstructionList* list = createList();

  for (int i = 0; i < list1->inst_num; i++)
  {
    addInstToList(list1->instructions[i], list);
  }

  for (int i = 0; i < list2->inst_num; i++)
  {
    addInstToList(list2->instructions[i], list);
  }

  return list;
  
}

void addInstToList(Instruction* inst, InstructionList* list) {
  list->inst_num++;

  if(list->instructions == NULL) {
    list->instructions = (Instruction**) malloc(sizeof(Instruction*));
    list->instructions[0] = inst;
  } else { //append no final da lista de instruçoes
    list->instructions = (Instruction**) realloc(list->instructions, sizeof(Instruction*) * list->inst_num);
    list->instructions[list->inst_num-1] = inst;
  }
}

Instruction* createInstruction(OpCode op, int arg1, int arg2, int arg3) {
  Instruction* inst = (Instruction*) malloc(sizeof(Instruction));

  inst->op_code = op;
  inst->arg1 = arg1;
  inst->arg2 = arg2;
  inst->arg3 = arg3;

  return inst;
}

//###############

