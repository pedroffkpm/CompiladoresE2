#include "instructions.h"

int register_counter = -1; //começa em 0
int label_counter = 0; //começa em 1 pq 0 é reservado pra main() 

//################

void insertHeadInstruction(Instruction inst, IList **head) {
  IList *novo = NULL;
  novo = (IList*) malloc(sizeof(*novo));

  if(novo) {
    novo->inst.op_code = inst.op_code;
    novo->inst.arg1 = inst.arg1;
    novo->inst.arg2 = inst.arg2;
    novo->inst.arg3 = inst.arg3;

    novo->next = *head;
    *head = novo;
  }
}

void insertTailInstruction(Instruction inst, IList **head) {
  //pega o ultimo
  IList *novo = NULL;
  novo = (IList*) malloc(sizeof(*novo));

  if (novo != NULL) {
  
    novo->inst.op_code = inst.op_code;
    novo->inst.arg1 = inst.arg1;
    novo->inst.arg2 = inst.arg2;
    novo->inst.arg3 = inst.arg3;

    novo->next = NULL;
  }

  IList *ult = *head;

  if(ult != NULL) {
    while (ult->next) {
      ult = ult->next;
    }

    ult->next = novo;
  } else {
    if(novo != NULL) {
      *head = novo;
    } else {
      *head = NULL;
    }
    
  }
}

void concatInstructions(IList **dest, IList **src) {
  IList *aux = *src;

  while(aux) {
    insertTailInstruction(aux->inst, dest);
    aux = aux->next;
  }
}

void getNodeByOp(IList *head, OpCode op, IList **node) {
  IList *aux = head;

  while (aux) {
    if(aux->inst.op_code == op)
      break;
    
    aux = aux->next;
  }
  
  if(node) {
    *node = aux;
  }
}

void freeList(IList** head) {
  IList *del;
  while (*head) {
    del  = *head;

    *head = (*head)->next;
    free(del);
    
  }
}

//################

Instruction lbl(int label) {
  return createInstruction(LBL, label, 0, 0);
}

Instruction jumpI(int label) {
  return createInstruction(JUMPI, label, 0, 0);
}

Instruction returnJump(int label) {
  return createInstruction(RETJUMP, label, 0, 0);
}

Instruction jump(int reg) {
  return createInstruction(JUMP, reg, 0, 0);
}

Instruction halt() {
  return createInstruction(HALT, 0, 0, 0);
}

Instruction addI(int reg, int op2, int dst) { //addI reg, c2 => r3 | dst = reg + op2
    return createInstruction(ADDI, reg, op2, dst);
}

Instruction loadI(int c, int reg) { //loadI c => reg | reg = c
    return createInstruction(LOADI, c, reg, 0);
}

Instruction loadAI(int src, int offset, int dst) { //loadAI src, offset => dest | dest = Memoria(src + offset)
    return createInstruction(LOADAI, src, offset, dst);
}

Instruction storeAI(int src, int dst, int offset) {
    return createInstruction(STOREAI, src, dst, offset);
}

Instruction i2i(int r1, int r2) {
    return createInstruction(I2I, r1, r2, 0);
}

Instruction cbr(int r, int label_true, int label_false) {
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


Instruction createInstruction(OpCode op, int arg1, int arg2, int arg3) {
  Instruction inst;

  inst.op_code = op;
  inst.arg1 = arg1;
  inst.arg2 = arg2;
  inst.arg3 = arg3;

  return inst;
}

//###############

