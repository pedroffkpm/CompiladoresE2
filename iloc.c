#include "iloc.h"

int register_counter = 0;
int label_counter = 0;

InstructionList* createList() {
  InstructionList* list = (InstructionList*) malloc((sizeof(InstructionList)));
  list->inst_num = 0;
  list->instructions = NULL;

  return list;
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
  inst->label = -1; //sinaliza que n tem label ainda

  return inst;
}

void assignCode(Node* node) {
  node->instructions = concatLists(node->instructions, node->kids[1]->instructions);
  int exprReg = register_counter; //reg tá com o último valor, que é o da expressao

  Symbol* aux = getSymbol(node->kids[0]->token->value.str);

  if(aux != NULL) {
    int reg = getRegister();
    if(getScopeForKey(aux->key) == 0) {
      addInstToList(addI(RBSS, aux->offset, reg), node->instructions);
    } else {
      addInstToList(addI(RFP, aux->offset, reg), node->instructions);
    }

    addInstToList(storeAI(exprReg, reg, 0), node->instructions);
  }

  //get address
  //addInst LOAD
}


//################

Instruction* addI(int op1, int op2, int dst) {
    return createInstruction(ADDI, op1, op2, dst);
}
Instruction* rsubI(int reg, int op, int dst) {
    return createInstruction(RSUBI, reg, op, dst);
}
Instruction* loadI(int value, int reg) {
    return createInstruction(LOADI, value, reg, 0);
}
Instruction* loadAI(int src, int offset, int dst) {
    return createInstruction(LOADAI, src, offset, dst);
}
Instruction* storeAI(int src, int dst, int offset) {
    return createInstruction(STOREAI, src, dst, offset);
}
Instruction* i2i(int r1, int r2) {
    return createInstruction(I2I, r1, r2, 0);
}

int getRegister() {
  register_counter++;
  return register_counter;
}

int getLabel() {
  label_counter++;
  return label_counter;
}