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

//#################################

void intCode(Node* node) {
  node->regTemp = getRegister();

  addInstToList(loadI(node->token->value.i, node->regTemp), node->instructions); //valor do int está em node->regTemp
}

void loadVarToRegCode(Node* node) {
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

void binOpCode(Node* node) { //unico jeito de pegar o tipo de operacao
  //operacoes binarias vao usar os registradores temporarios dos nodos filhos

  switch (node->token->tokenType)
  {
  case SPEC_CHAR:
    if((node->token->value.c == '<') || (node->token->value.c == '>') || (node->token->value.c == '!')) {
      booleanCode(node);
    }
    arithmeticCode(node);
    break;
  case COMP_OPER:
    booleanCode(node); //shift right e shift left são aritméticos
    break;
  default:
    break;
  }
}

void arithmeticCode(Node* node) { //pra diferenciar a op tem que fazer ifs

OpCode op = NOP;
  if(node->token->value.c == '+') {
    op = ADD; //versao das operacoes que so usam regs
  }
  if(node->token->value.c == '-') {
    op = SUB; //versao das operacoes que so usam regs
  }
  if(node->token->value.c == '/') {
    op = DIV; //versao das operacoes que so usam regs
  }
  if(node->token->value.c == '*') {
    op = MULT; //versao das operacoes que so usam regs
  }

  node->instructions = concatLists(node->kids[0]->instructions, node->kids[1]->instructions); // E.code = E1.code || E2.code
  node->regTemp = getRegister();

  addInstToList(createInstruction(op, node->kids[0]->regTemp, node->kids[1]->regTemp, node->regTemp), node->instructions);
}

void booleanCode(Node* node) { //pra diferenciar => strcmp
  if (node->token->value.c == '<') {
    relopCode(node, CMP_LT);
  }

  else if (strcmp(node->token->value.str, "<=") == 0) {
    relopCode(node, CMP_LE);
  }

  else if (strcmp(node->token->value.str, "==") == 0) {
    relopCode(node, CMP_EQ);
  }

  else if (strcmp(node->token->value.str, ">=") == 0) {
    relopCode(node, CMP_GE);
  }

  else if (node->token->value.c == '>') {
    relopCode(node, CMP_GT);
  }

  else if (strcmp(node->token->value.str, "!=") == 0) {
      relopCode(node, CMP_NE);
  }

  else {
    logicCode(node);
  }

}

void relopCode(Node* node, OpCode op) {

  int trueLabel = getLabel();
  int falseLabel = getLabel();

  addTrueList(node, trueLabel);
  addFalseList(node, falseLabel);

  node->instructions = concatLists(node->kids[0]->instructions, node->kids[1]->instructions);
  node->regTemp = getRegister();

  addInstToList(createInstruction(op, node->kids[0]->regTemp, node->kids[1]->regTemp, node->regTemp), node->instructions);
  addInstToList(cbr(node->regTemp, trueLabel, falseLabel), node->instructions);

}

void remendaTrue(Node* node, int newLabel) {
  for (int i = 0; i < node->trueListSize; i++)
  { //trocar node->tl[i] por newLabel
  for (int j = node->instructions->inst_num-1; j >= 0; j--)
  {//segundo campo das cbr
    if(node->instructions->instructions[j]->op_code == CBR) {
      if(node->instructions->instructions[j]->arg2 == node->tl[i]) {
        Instruction* newCBR = cbr(node->instructions->instructions[j]->arg1, newLabel, node->instructions->instructions[j]->arg3);
        free(node->instructions->instructions[j]);
        node->instructions->instructions[j] = newCBR;
        newCBR = NULL;
      }
    }
  }
  }
}

void remendaFalse(Node* node, int newLabel) {
  for (int i = 0; i < node->falseListSize; i++)
  { //trocar node->fl[i] por newLabel
  for (int j = node->instructions->inst_num-1; j >= 0; j--)
  {//itera por todas as instrucoes
    if(node->instructions->instructions[j]->op_code == CBR) { //se for CBR
      if(node->instructions->instructions[j]->arg3 == node->fl[i]) { //se o arg3 for uma das labels a ser trocada
      Instruction* newCBR = cbr(node->instructions->instructions[j]->arg1, node->instructions->instructions[j]->arg2, newLabel);
      free(node->instructions->instructions[j]); //funciona?
        node->instructions->instructions[j] = newCBR;
        newCBR = NULL;
      }
    }
  }
  }
}

void logicCode(Node* node) {

  int x = getLabel();

  if(strcmp(node->token->value.str, "&&") == 0) {
    remendaTrue(node->kids[0], x);
    concatTrueL(node, node->kids[1]); // B.tl = B2.tl
    concatFalseL(node, node->kids[0]); //B.fl = NULL || B1.fl
    concatFalseL(node, node->kids[1]); //B.fl = B1.fl || B2.fl
  }

  if(strcmp(node->token->value.str, "||") == 0) {
    remendaFalse(node->kids[0], x);
    concatFalseL(node, node->kids[1]); //B.fl = B2.fl
    concatTrueL(node, node->kids[0]); //B.tl = NULL || B1.tl
    concatTrueL(node, node->kids[1]); //B.tl = B1.tl || B2.tl
  }
  node->instructions = concatLists(node->kids[0]->instructions, node->instructions); //B.code = B1.code
  addInstToList(createInstruction(LBL, x, 0, 0), node->instructions); //B.code = B1.code || "Lx: "
  node->instructions = concatLists(node->instructions, node->kids[1]->instructions); //B.code = B1.code || "Lx: " || B2.code

}

//###########################

char* special_register(Register nmr) {
  switch (nmr)
  {
  case RFP:
    return "rfp";
    break;
  case RSP:
    return "rsp";
    break;
  case RBSS:
    return "rbss";
    break;
  case RPC:
    return "rpc";
    break;
  default:
    return "";
    break;
  }
}

void printCode(InstructionList list) {
  for (int i = 0; i < list.inst_num; i++)
  {
    printInstruction(list.instructions[i]);
  }
}

void printInstruction(Instruction *instruction)
{
  switch (instruction->op_code)
  {
  case LBL:
    printf("L%d \n", instruction->arg1);
    break;
  case NOP:
    printf("nop \n");
    break;
  case ADD:
    printf("add r%d, r%d => r%d\n", instruction->arg1, instruction->arg2, instruction->arg3);
    break;
  case ADDI:
    printf("addI r%d, %d => r%d\n", instruction->arg1, instruction->arg2, instruction->arg3);
    break;
  case SUB:
    printf("sub r%d, r%d => r%d\n",  instruction->arg1, instruction->arg2, instruction->arg3);
    break;
  case SUBI:
    printf("subI r%d, %d => r%d\n", instruction->arg1, instruction->arg2, instruction->arg3);
    break;
  case RSUBI:
    printf("rsubI r%d, %d => r%d\n", instruction->arg1, instruction->arg2, instruction->arg3);
    break;
  case MULT:
    printf("mult r%d, r%d => r%d\n", instruction->arg1, instruction->arg2, instruction->arg3);
    break;
  case MULTI:
    printf("multI r%d, %d => r%d\n", instruction->arg1, instruction->arg2, instruction->arg3);
    break;
  case DIV:
    printf("div r%d, r%d => r%d\n", instruction->arg1, instruction->arg2, instruction->arg3);
    break;
  case DIVI:
    printf("divI r%d, %d => r%d\n", instruction->arg1, instruction->arg2, instruction->arg3);
    break;
  case RDIVI:
    printf("rdivI r%d, %d => r%d\n", instruction->arg1, instruction->arg2, instruction->arg3);
    break;
  case LOADI: // r2 = c1 | constante
    if(instruction->arg2 < 0)
      printf("loadI %d => %s\n", instruction->arg1, special_register(instruction->arg2));
    else
      printf("loadI %d => r%d\n", instruction->arg1, instruction->arg2);
    break;
  case LOADA0: //só usar especial como 1o arg
    if(instruction->arg1 < 0)
      printf("loadA0 %s, r%d => r%d\n", special_register(instruction->arg1), instruction->arg2, instruction->arg3);
    else
      printf("loadA0 r%d, r%d => r%d\n", instruction->arg1, instruction->arg2, instruction->arg3);
    break;
  case LOADAI: // r3 = Mem(r1 + c2) | constante/offset
    if(instruction->arg1 < 0)
      printf("loadAI %s, %d => r%d\n", special_register(instruction->arg1), instruction->arg2, instruction->arg3);
    else //acho que só é usado com os especiais
      printf("loadAI r%d, %d => r%d\n", instruction->arg1, instruction->arg2, instruction->arg3);
    break;
  case STOREAI: // storeAI r1 => r2, c3 | Mem(r2 + c3) = r1
    if(instruction->arg2 < 0)
      printf("storeAI r%d => %s, %d\n", instruction->arg1, special_register(instruction->arg2), instruction->arg3);
    else
      printf("storeAI r%d => r%d, %d\n", instruction->arg1, instruction->arg2, instruction->arg3);
    break;
  case STOREA0:
    if(instruction->arg2 < 0)
      printf("storeA0 r%d => %s, r%d\n", instruction->arg1, special_register(instruction->arg2), instruction->arg3);
    else
      printf("storeA0 r%d => r%d, r%d\n", instruction->arg1, instruction->arg2, instruction->arg3);
    break;
  case I2I: //i2i r1 => r2 | r1 = r2 para inteiros
    if (instruction->arg1 < 0 && instruction->arg2 < 0) {
      printf("i2i %s => %s\n", special_register(instruction->arg1), special_register(instruction->arg2));
    } else if (instruction->arg1 < 0) {
      printf("i2i %s => r%d\n", special_register(instruction->arg1), instruction->arg2);
    } else if (instruction->arg2 < 0) {
      printf("i2i r%d => %s\n", instruction->arg1, special_register(instruction->arg2));
    } else {
      printf("i2i r%d => r%d\n", instruction->arg1, instruction->arg2);
    }
    break;
  case JUMPI:
    printf("jumpI -> L%d\n", instruction->arg1);
    break;
  case JUMP:
    printf("jump -> r%d\n", instruction->arg1);
    break;
  case CBR: // cbr r1 -> l2, l3 | r1 ? pc = endereco(l2) : pc=endereco(l3)
    printf("cbr r%d -> L%d, L%d\n", instruction->arg1, instruction->arg2, instruction->arg3);
    break;
  case CMP_LT:
    printf("cmp_LT r%d, r%d -> r%d\n", instruction->arg1, instruction->arg2, instruction->arg3);
    break;
  case CMP_LE:
    printf("cmp_LE r%d, r%d -> r%d\n", instruction->arg1, instruction->arg2, instruction->arg3);
    break;
  case CMP_EQ:
    printf("cmp_EQ r%d, r%d -> r%d\n", instruction->arg1, instruction->arg2, instruction->arg3);
    break;
  case CMP_GE:
    printf("cmp_GE r%d, r%d -> r%d\n", instruction->arg1, instruction->arg2, instruction->arg3);
    break;
  case CMP_GT:
    printf("cmp_GT r%d, r%d -> r%d\n", instruction->arg1, instruction->arg2, instruction->arg3);
    break;
  case CMP_NE:
    printf("cmp_NE r%d, r%d -> r%d\n", instruction->arg1, instruction->arg2, instruction->arg3);
    break;
  case HALT:
    printf("halt\n");
    break;
  default:
    break;
  }
}
