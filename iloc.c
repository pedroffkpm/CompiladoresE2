#include "iloc.h"


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

void remendaTrue(Node *node, int newLabel)
{
  for (int i = 0; i < node->trueNmr; i++) { //trocar node->tl[i] por newLabel
    for (int j = node->instructions->inst_num - 1; j >= 0; j--) { //segundo campo das cbr
      if (node->instructions->instructions[j]->op_code == CBR) {
        if (node->instructions->instructions[j]->arg2 == node->tl[i]) {
          node->instructions->instructions[j]->arg2 = newLabel;
        }
      }
    }
  }
}

//x && y && z && w

void remendaFalse(Node *node, int newLabel)
{
  for (int i = 0; i < node->falseNmr; i++)
  { //trocar node->fl[i] por newLabel
    for (int j = node->instructions->inst_num - 1; j >= 0; j--)
    { //itera por todas as instrucoes
      if (node->instructions->instructions[j]->op_code == CBR)
      { //se for CBR
        if (node->instructions->instructions[j]->arg3 == node->fl[i])
        { //se o arg3 for uma das labels a ser trocada
          node->instructions->instructions[j]->arg3 = newLabel;
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
  addInstToList(lbl(x), node->instructions); //B.code = B1.code || "Lx: "
  node->label = x; //esse tipo de instrução TEM uma label! 
  node->instructions = concatLists(node->instructions, node->kids[1]->instructions); //B.code = B1.code || "Lx: " || B2.code

}

void ternOpCode(Node* node) { // (kids[1]/condition) ? (kids[0]/if) : (kids[2]/else)
//int x = (5 == 5) ? 3 : 2;
//preciso pegar o registrador do tern pra usar no assign
//preciso passar o valor do registrador que avaliou (true ou false) pra cá

  int x = getLabel();
  int y = getLabel();

  remendaTrue(node->kids[1], x);
  remendaFalse(node->kids[1], y);
  node->label = getLabel();
  node->regTemp = getRegister(); //precisa achar um jeito de passar o resultado pra cá

  node->instructions = concatLists(node->kids[1]->instructions, node->instructions); //S.code = B.code
  addInstToList(lbl(x), node->instructions); // S.code = B.code || "Lx: "
  node->instructions = concatLists(node->instructions, node->kids[0]->instructions); // S.code = B.code || "Lx: " || S1.code
  addInstToList(i2i(node->kids[0]->regTemp, node->regTemp), node->instructions); // pega o valor do registrador true
  addInstToList(jumpI(node->label), node->instructions); // || jump Lz
  addInstToList(lbl(y), node->instructions); // || "Ly: "
  node->instructions = concatLists(node->instructions, node->kids[2]->instructions);
  addInstToList(i2i(node->kids[2]->regTemp, node->regTemp), node->instructions);
  addInstToList(jumpI(node->label), node->instructions); // || jump Lz
  addInstToList(lbl(node->label), node->instructions);
  //S.code = B.code || "Lx" || S1.code || i2i S1.reg S.reg || jump Lz || "Ly" || S2.code || i2i S2.reg S.reg || jump Lz || "Lz"

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

void printCode(InstructionList* list) {
  for (int i = 0; i < list->inst_num; i++)
  {
    printInstruction(list->instructions[i]);
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
