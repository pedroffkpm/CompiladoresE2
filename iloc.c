#include "iloc.h"

int register_counter = 0;
int label_counter = 0;

//################

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

//#################################

void intCode(Node* node) {
  node->regTemp = getRegister();

  addInstToList(loadI(node->token->value.i, node->regTemp), node->instructions); //valor do int está em node->regTemp
}

void loadVarToReg(Node* node) {
  node->regTemp = getRegister();

  Symbol* aux = getSymbol(node->token->value.str);

  if(aux != NULL) {
    int x = getRegister();
    if(getScopeForKey(aux->key) == 0) {
      addInstToList(addI(RBSS, aux->offset, x), node->instructions);
    } else {
      addInstToList(addI(RFP, aux->offset, x), node->instructions);
    }
    //x contém end(var)

    addInstToList(loadAI(x, 0, node->regTemp), node->instructions); //node->temp = Mem(x + 0)
  }

  aux = NULL;

}


void assignCode(Node* node) {
  node->instructions = concatLists(node->instructions, node->kids[1]->instructions);
  int exprReg = node->kids[1]->regTemp;

  Symbol* aux = getSymbol(node->kids[0]->token->value.str);

  if(aux != NULL) {
    node->regTemp = getRegister();
    if(getScopeForKey(aux->key) == 0) {
      addInstToList(addI(RBSS, aux->offset, node->regTemp), node->instructions); // end(id) => node->regTemp
    } else {
      addInstToList(addI(RFP, aux->offset, node->regTemp), node->instructions);
    }

    addInstToList(storeAI(exprReg, node->regTemp, 0), node->instructions); // Mem(end(id) + 0) = exprReg
  }

  aux = NULL;
  //get address
  //addInst LOAD
}