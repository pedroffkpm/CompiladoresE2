#include "iloc.h"


int return_label;
bool has_main = false;
//#################################

void intCode(Node* node) {
  node->regTemp = getRegister();

  insertTailInstruction(loadI(node->token->value.i, node->regTemp), &(node->inst_head));
  // addInstToNode(loadI(node->token->value.i, node->regTemp), node, false);

  // addInstToList(loadI(node->token->value.i, node->regTemp), node->instructions); //valor do int está em node->regTemp
}

void loadVarToRegCode(Node* node) {
  node->regTemp = getRegister();

  Symbol* aux = getSymbol(node->token->value.str);

  if(aux != NULL) {
    int x = getRegister();
    if(getScopeForKey(aux->key) == 0) {
      insertTailInstruction(addI(RBSS, aux->offset, x), &(node->inst_head));
      // addInstToNode(addI(RBSS, aux->offset, x), node, false);
      // addInstToList(addI(RBSS, aux->offset, x), node->instructions);
    } else {
      insertTailInstruction(addI(RFP, aux->offset, x), &(node->inst_head));
      // addInstToNode(addI(RFP, aux->offset, x), node, false);
      // addInstToList(addI(RFP, aux->offset, x), node->instructions);
    }
    //x contém end(var)

    insertTailInstruction(loadAI(x, 0, node->regTemp), &(node->inst_head));
    // addInstToNode(loadAI(x, 0, node->regTemp), node, false);
    // addInstToList(loadAI(x, 0, node->regTemp), node->instructions); //node->temp = Mem(x + 0)
  }

  aux = NULL;
}

void assignCode(Node* node) {
  concatInstructions(&(node->inst_head), &(node->kids[1]->inst_head));
  // concatInstructions(node, node->kids[1], false);
  // node->instructions = concatLists(node->instructions, node->kids[1]->instructions);
  int exprReg = node->kids[1]->regTemp;

  Symbol* aux = getSymbol(node->kids[0]->token->value.str);

  if(aux != NULL) {
    node->regTemp = getRegister();
    if(getScopeForKey(aux->key) == 0) {
      insertTailInstruction(addI(RBSS, aux->offset, node->regTemp), &(node->inst_head));
      // addInstToNode(addI(RBSS, aux->offset, node->regTemp), node, false);
      // addInstToList(addI(RBSS, aux->offset, node->regTemp), node->instructions); // end(id) => node->regTemp
    } else {
      insertTailInstruction(addI(RFP, aux->offset, node->regTemp), &(node->inst_head));
      // addInstToNode(addI(RFP, aux->offset, node->regTemp), node, false);
      // addInstToList(addI(RFP, aux->offset, node->regTemp), node->instructions);
    }

    insertTailInstruction(storeAI(exprReg, node->regTemp, 0), &(node->inst_head));
    // addInstToNode(storeAI(exprReg, node->regTemp, 0), node, false);
    // addInstToList(storeAI(exprReg, node->regTemp, 0), node->instructions); // Mem(end(id) + 0) = exprReg
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

  concatInstructions(&(node->inst_head), &(node->kids[0]->inst_head));
  // concatInstructions(node, node->kids[0], false);
  concatInstructions(&(node->inst_head), &(node->kids[1]->inst_head));
  // concatInstructions(node, node->kids[1], false);
  // node->instructions = concatLists(node->kids[0]->instructions, node->kids[1]->instructions); // E.code = E1.code || E2.code
  node->regTemp = getRegister();

  insertTailInstruction(createInstruction(op, node->kids[0]->regTemp, node->kids[1]->regTemp, node->regTemp), &(node->inst_head));
  // addInstToNode(createInstruction(op, node->kids[0]->regTemp, node->kids[1]->regTemp, node->regTemp), node, false);
  // addInstToList(createInstruction(op, node->kids[0]->regTemp, node->kids[1]->regTemp, node->regTemp), node->instructions);
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

  concatInstructions(&(node->inst_head), &(node->kids[0]->inst_head));
  // concatInstructions(node, node->kids[0], false); //S.code = S1.cde
  concatInstructions(&(node->inst_head), &(node->kids[1]->inst_head));
  // concatInstructions(node, node->kids[1], false); //S.code = S1.code || S2.code
  // node->instructions = concatLists(node->kids[0]->instructions, node->kids[1]->instructions);
  node->regTemp = getRegister();

  insertTailInstruction(createInstruction(op, node->kids[0]->regTemp, node->kids[1]->regTemp, node->regTemp), &(node->inst_head));
  // addInstToNode(createInstruction(op, node->kids[0]->regTemp, node->kids[1]->regTemp, node->regTemp), node, false);
  // addInstToList(createInstruction(op, node->kids[0]->regTemp, node->kids[1]->regTemp, node->regTemp), node->instructions);
  insertTailInstruction(cbr(node->regTemp, trueLabel, falseLabel), &(node->inst_head));
  // addInstToNode(cbr(node->regTemp, trueLabel, falseLabel), node, false);
  // addInstToList(cbr(node->regTemp, trueLabel, falseLabel), node->instructions);
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

  concatInstructions(&(node->inst_head), &(node->kids[0]->inst_head));
  // concatInstructions(node, node->kids[0], true);
  // node->instructions = concatLists(node->kids[0]->instructions, node->instructions); //B.code = B1.code
  insertTailInstruction(lbl(x), &(node->inst_head));
  // addInstToNode(lbl(x), node, false);
  // addInstToList(lbl(x), node->instructions); //B.code = B1.code || "Lx: "
  node->label = x; //esse tipo de instrução TEM uma label!
  concatInstructions(&(node->inst_head), &(node->kids[1]->inst_head));
  // concatInstructions(node, node->kids[1], false);
  // node->instructions = concatLists(node->instructions, node->kids[1]->instructions); //B.code = B1.code || "Lx: " || B2.code

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

  concatInstructions(&(node->inst_head), &(node->kids[1]->inst_head));
  // concatInstructions(node, node->kids[1], false);
  // node->instructions = concatLists(node->kids[1]->instructions, node->instructions); //S.code = B.code
  insertTailInstruction(lbl(x), &(node->inst_head));
  // addInstToNode(lbl(x), node, false);
  // addInstToList(lbl(x), node->instructions); // S.code = B.code || "Lx: "
  concatInstructions(&(node->inst_head), &(node->kids[0]->inst_head));
  // concatInstructions(node, node->kids[0], false);
  // node->instructions = concatLists(node->instructions, node->kids[0]->instructions); // S.code = B.code || "Lx: " || S1.code
  insertTailInstruction(i2i(node->kids[0]->regTemp, node->regTemp), &(node->inst_head));
  // addInstToNode(i2i(node->kids[0]->regTemp, node->regTemp), node, false);
  // addInstToList(i2i(node->kids[0]->regTemp, node->regTemp), node->instructions); // pega o valor do registrador true
  insertTailInstruction(jumpI(node->label), &(node->inst_head));
  // addInstToNode(jumpI(node->label), node, false);
  // addInstToList(jumpI(node->label), node->instructions); // || jump Lz
  insertTailInstruction(lbl(y), &(node->inst_head));
  // addInstToNode(lbl(y), node, false);
  // addInstToList(lbl(y), node->instructions); // || "Ly: "
  concatInstructions(&(node->inst_head), &(node->kids[2]->inst_head));
  // concatInstructions(node, node->kids[2], false);
  // node->instructions = concatLists(node->instructions, node->kids[2]->instructions);
  insertTailInstruction(i2i(node->kids[2]->regTemp, node->regTemp), &(node->inst_head));
  // addInstToNode(i2i(node->kids[2]->regTemp, node->regTemp), node, false);
  // addInstToList(i2i(node->kids[2]->regTemp, node->regTemp), node->instructions);
  insertTailInstruction(jumpI(node->label), &(node->inst_head));
  // addInstToNode(jumpI(node->label), node, false);
  // addInstToList(jumpI(node->label), node->instructions); // || jump Lz
  insertTailInstruction(lbl(node->label), &(node->inst_head));
  // addInstToNode(lbl(node->label), node, false);
  // addInstToList(lbl(node->label), node->instructions);
  //S.code = B.code || "Lx" || S1.code || i2i S1.reg S.reg || jump Lz || "Ly" || S2.code || i2i S2.reg S.reg || jump Lz || "Lz"

}

void ifElseCode(Node* node) { //kids[0] = cond ; kids[1] = then ; kids[2] = else(pode ser NULL)

  int x = getLabel();
  int y = getLabel();

  remendaTrue(node->kids[0], x);
  remendaFalse(node->kids[0], y);
  // node->label = getLabel();

  concatInstructions(&(node->inst_head), &(node->kids[0]->inst_head));
  // concatInstructions(node, node->kids[0], false);
  // node->instructions = concatLists(node->kids[0]->instructions, node->instructions); //S.code = B.code
  insertTailInstruction(lbl(x), &(node->inst_head));
  // addInstToNode(lbl(x), node, false);
  // addInstToList(lbl(x), node->instructions); // S.code = B.code || "Lx: "
  concatInstructions(&(node->inst_head), &(node->kids[1]->inst_head));
  // concatInstructions(node, node->kids[1], false);
  // node->instructions = concatLists(node->instructions, node->kids[1]->instructions); // B.code || Lx || S1.code

  if(node->kidsNumber == 4) { //TEM ELSE
  node->label = getLabel();
  insertTailInstruction(jumpI(node->label), &(node->inst_head));
  // addInstToNode(jumpI(node->label), node, false);
  // addInstToList(jumpI(node->label), node->instructions); //B.code || Lx || S1.code || jump Lz
  insertTailInstruction(lbl(y), &(node->inst_head));
  // addInstToNode(lbl(y), node, false);
  // addInstToList(lbl(y), node->instructions); // B.code || Lx || S1.code || jump Lz || Ly
  concatInstructions(&(node->inst_head), &(node->kids[2]->inst_head));
  // concatInstructions(node, node->kids[2], false);
  // node->instructions = concatLists(node->instructions, node->kids[2]->instructions); // B.code || Lx || S1.code || jump Lz || Ly || S2.code
  insertTailInstruction(jumpI(node->label), &(node->inst_head));
  // addInstToNode(jumpI(node->label), node, false);
  // addInstToList(jumpI(node->label), node->instructions); // jump Lz
  insertTailInstruction(lbl(node->label), &(node->inst_head));
  // addInstToNode(lbl(node->label), node, false);
  // addInstToList(lbl(node->label), node->instructions);
  //B.code || Lx || S1.code || jump z || Ly || S2.code || jump z || Lz || <prox. instrucoes>
  } else {
    //se n tem else, pulou pro label de FALSE
    insertTailInstruction(lbl(y), &(node->inst_head));
    // addInstToNode(lbl(y), node, false);
    // addInstToList(lbl(y), node->instructions); // B.code || LX || S1.code || Ly
  }
  //acabaria aqui
  //OU Lz vai ser o prox. nodo; OU Ly vai
  concatInstructions(&(node->inst_head), &(node->kids[node->kidsNumber-1]->inst_head));
  // concatInstructions(node, node->kids[node->kidsNumber-1], false);
  // node->instructions = concatLists(node->instructions, node->kids[node->kidsNumber-1]->instructions);
  // sem ELSE:
    // B.code || Lx || S1.code || Ly || <prox.>
  // com ELSE:
    // B.code || Lx || S1.code || jump z || Ly || S2.code || jump z || Lz || <prox.>
}

void whileCode(Node* node) { //kids[0] = cond ; kids[1] = bloco
  node->label = getLabel();

  int x = getLabel();
  int y = getLabel();

  remendaTrue(node->kids[0], x);
  remendaFalse(node->kids[0], y);

  insertTailInstruction(lbl(node->label), &(node->inst_head));
  // addInstToNode(lbl(node->label), node, false);
  // addInstToList(lbl(node->label), node->instructions); // S.code = "Lz"
  concatInstructions(&(node->inst_head), &(node->kids[0]->inst_head));
  // concatInstructions(node, node->kids[0], false);
  // node->instructions = concatLists(node->instructions, node->kids[0]->instructions); // S.code = Lz || B.code
  insertTailInstruction(lbl(x), &(node->inst_head));
  // addInstToNode(lbl(x), node, false);
  // addInstToList(lbl(x), node->instructions); // Lz || B.code || Lx
  concatInstructions(&(node->inst_head), &(node->kids[1]->inst_head));
  // concatInstructions(node, node->kids[1], false);
  // node->instructions = concatLists(node->instructions, node->kids[1]->instructions); // Lz || B.code || Lx || S1.code
  insertTailInstruction(jumpI(node->label), &(node->inst_head));
  // addInstToNode(jumpI(node->label), node, false);
  // addInstToList(jumpI(node->label), node->instructions); // Lz || B.code || Lx || S1.code || jump z
  insertTailInstruction(lbl(y), &(node->inst_head));
  // addInstToNode(lbl(y), node, false);
  // addInstToList(lbl(y), node->instructions); // Lz || B.code || Lx || S1.code || jump z || Ly

}

void forCode(Node* node) {
  //for ( kids[0]; kids[1]; kids[2]) kids[3]
  //[int i = 0]; teste: [i < 5; cbr true false]; true: [printf("%d", i); i++; jump teste]; false: [nop...]

  concatInstructions(&(node->inst_head), &(node->kids[0]->inst_head));
  // concatInstructions(node, node->kids[0], true); //append no inicio

  // node->instructions = concatLists(node->kids[0]->instructions, node->instructions); // S.code = S1.code

  node->label = getLabel();
  int x = getLabel();
  int y = getLabel();

  remendaTrue(node->kids[1], x); //remenda B
  remendaFalse(node->kids[1], y); //remenda B

  insertTailInstruction(lbl(node->label), &(node->inst_head));
  // addInstToNode(lbl(node->label), node, false);
  // addInstToList(lbl(node->label), node->instructions); // S1.code || Lz: ||
  concatInstructions(&(node->inst_head), &(node->kids[1]->inst_head));
  // concatInstructions(node, node->kids[1], false);
  // node->instructions = concatLists(node->instructions, node->kids[1]->instructions); //S1.code || Lz (teste) || B.code

  insertTailInstruction(lbl(x), &(node->inst_head));
  // addInstToNode(lbl(x), node, false);
  // addInstToList(lbl(x), node->instructions); // S1.code || L teste || B.code || L true
  concatInstructions(&(node->inst_head), &(node->kids[3]->inst_head));
  // concatInstructions(node, node->kids[3], false);
  // node->instructions = concatLists(node->instructions, node->kids[3]->instructions); //S1 || teste || B || true || S3
  concatInstructions(&(node->inst_head), &(node->kids[2]->inst_head));
  // concatInstructions(node, node->kids[2], false);
  // node->instructions = concatLists(node->instructions, node->kids[2]->instructions); //S1 || teste || B || true || S3 || S2
  insertTailInstruction(jumpI(node->label), &(node->inst_head));
  // addInstToNode(jumpI(node->label), node, false);
  // addInstToList(jumpI(node->label), node->instructions);// S1 || Lz || B || Lx || S3 || S2 || jump z

  insertTailInstruction(lbl(y), &(node->inst_head));
  // addInstToNode(lbl(y), node, false);
  // addInstToList(lbl(y), node->instructions);
  //S1 || Lz || B (cbr Lx Ly) || Lx: || S3 || S2 || jump z || Ly: 

}

void functionDeclarationCode(Node* node) {
  if(strcmp(node->token->value.str, "main") == 0) {
    has_main = true;
    node->label = 0; //especial pra main
    insertTailInstruction(lbl(node->label), &(node->inst_head));
    // addInstToNode(lbl(node->label), node, false);
    // addInstToList(lbl(node->label), node->instructions);
    concatInstructions(&(node->inst_head), &(node->kids[0]->inst_head));
    // concatInstructions(node, node->kids[0], false);
    // node->instructions = concatLists(node->instructions, node->kids[0]->instructions);
  } else {
    Symbol* s = getSymbol(node->token->value.str);
    node->label = getLabel();
    insertTailInstruction(lbl(node->label), &(node->inst_head));
    // addInstToNode(lbl(node->label), node, false);
    // addInstToList(lbl(node->label), node->instructions);
    int rfp_copy = getRegister();

    insertTailInstruction(i2i(RFP, rfp_copy), &(node->inst_head));
    // addInstToNode(i2i(RFP, rfp_copy), node, false);
    // addInstToList(i2i(RFP, rfp_copy), node->instructions); //salva rfp original
    insertTailInstruction(i2i(RSP, RFP), &(node->inst_head));
    // addInstToNode(i2i(RSP, RFP), node, false);
    // addInstToList(i2i(RSP, RFP), node->instructions); //atualiza rfp
    insertTailInstruction(storeAI(rfp_copy, RFP, 8 + (s->n_params * 4)), &(node->inst_head));
    // addInstToNode(storeAI(rfp_copy, RFP, 8 + (s->n_params * 4)), node, false);
    // addInstToList(storeAI(rfp_copy, RFP, 8 + (s->n_params * 4)), node->instructions);
    //salva valor original de rfp em rfp + 8 + tamanho dos params

    int reg = getRegister();
    insertTailInstruction(loadI(0, reg), &(node->inst_head));
    // addInstToNode(loadI(0, reg), node, false);
    // addInstToList(loadI(0, reg), node->instructions);
    insertTailInstruction(storeAI(reg, RFP, 12 + (s->n_params * 4)), &(node->inst_head));
    // addInstToNode(storeAI(reg, RFP, 12 + (s->n_params * 4)), node, false);
    // addInstToList(storeAI(reg, RFP, 12 + (s->n_params * 4)), node->instructions);

    insertTailInstruction(addI(RSP, 16 + (s->n_params * 4), RSP), &(node->inst_head));
    // addInstToNode(addI(RSP, 16 + (s->n_params * 4), RSP), node, false);
    // addInstToList(addI(RSP, 16 + (s->n_params * 4), RSP), node->instructions);

    return_label = getLabel();
    //precisa setar a label de retorno antes de pegar as insts (onde pode ter um return)
    concatInstructions(&(node->inst_head), &(node->kids[0]->inst_head));
    // concatInstructions(node, node->kids[0], false); //pega insts do bloco
    remendaReturn(node); //troca a label pra return_label
    // node->instructions = concatLists(node->instructions, node->kids[0]->instructions);
    insertTailInstruction(lbl(return_label), &(node->inst_head));
    // addInstToNode(lbl(return_label), node, false);
    // addInstToList(lbl(return_label), node->instructions);

    int return_addr = getRegister();
    insertTailInstruction(loadAI(RFP, 4 + (s->n_params * 4), return_addr), &(node->inst_head));
    // addInstToNode(loadAI(RFP, 4 + (s->n_params * 4), return_addr), node, false);
    // addInstToList(loadAI(RFP, 4 + (s->n_params * 4), return_addr), node->instructions);

    int old_rfp = getRegister();
    insertTailInstruction(loadAI(RFP, 8 + (s->n_params * 4), old_rfp), &(node->inst_head));
    // addInstToNode(loadAI(RFP, 8 + (s->n_params * 4), old_rfp), node, false);
    // addInstToList(loadAI(RFP, 8 + (s->n_params * 4), old_rfp), node->instructions);

    insertTailInstruction(i2i(old_rfp, RFP), &(node->inst_head));
    // addInstToNode(i2i(old_rfp, RFP), node, false);
    // addInstToList(i2i(old_rfp, RFP), node->instructions);
    insertTailInstruction(i2i(RFP, RSP), &(node->inst_head));
    // addInstToNode(i2i(RFP, RSP), node, false);
    // addInstToList(i2i(RFP, RSP), node->instructions);
    insertTailInstruction(jump(return_addr), &(node->inst_head));
    // addInstToNode(jump(return_addr), node, false);
    // addInstToList(jump(return_addr), node->instructions);
  }

  setFuncLabel(node->token->value.str, node->label); //adiciona label na tabela
}

void returnCode(Node* node) {
  if(node->kidsNumber > 0) {
    concatInstructions(&(node->inst_head), &(node->kids[0]->inst_head));
    // concatInstructions(node, node->kids[0], false); //append ao final
    // node->instructions = concatLists(node->instructions, node->kids[0]->instructions);
    insertTailInstruction(storeAI(node->kids[0]->regTemp, RFP, 0), &(node->inst_head));
    // addInstToNode(storeAI(node->kids[0]->regTemp, RFP, 0), node, false); //empilha resultado
    // addInstToList(storeAI(node->kids[0]->regTemp, RFP, 0), node->instructions);
  }

  insertTailInstruction(returnJump(return_label), &(node->inst_head));
  // addInstToNode(returnJump(return_label), node, false);
  // addInstToList(jumpI(return_label), node->instructions);
}

void functionCallCode(Node* node) { //args (kids[0]) pode ser null
  int rsp_copy = getRegister();
  int return_address = getRegister();


  insertTailInstruction(i2i(RSP, rsp_copy), &(node->inst_head));
  // addInstToNode(i2i(RSP, rsp_copy), node, false);
  // addInstToList(i2i(RSP, rsp_copy), node->instructions); //rsp original
  insertTailInstruction(addI(RSP, 4, RSP), &(node->inst_head));
  // addInstToNode(addI(RSP, 4, RSP), node, false);
  // addInstToList(addI(RSP, 4, RSP), node->instructions); //aqui (rsp antes de add 4) vem o valor de retorno

  concatInstructions(&(node->inst_head), &(node->kids[0]->inst_head));
  // concatInstructions(node, node->kids[0], false); //append no final
  // node->instructions = concatLists(node->instructions, node->kids[0]->instructions);
  //codigo de todos os args COLOCADOS EM RSP (topo da pilha)

  insertTailInstruction(addI(RPC, 4, return_address), &(node->inst_head));
  // addInstToNode(addI(RPC, 4, return_address), node, false); //adiciona ao final
  // addInstToList(addI(RPC, 4, return_address), node->instructions); //rpc+4 (depois do jump)
  //endereço pra prox. instrucao; retornar depois do jump
  //addI + store + i2i + jump

  insertTailInstruction(storeAI(return_address, RSP, 0), &(node->inst_head));
  // addInstToNode(storeAI(return_address, RSP, 0), node, false);
  // addInstToList(storeAI(return_address, RSP, 0), node->instructions);
  //guarda endereço de retorno no topo da pilha

  insertTailInstruction(i2i(rsp_copy, RSP), &(node->inst_head));
  // addInstToNode(i2i(rsp_copy, RSP), node, false);
  // addInstToList(i2i(rsp_copy, RSP), node->instructions); //restora RSP (aqui está o valor de retorno)

  /* DESCOBRE LABEL DA FUNCAO */
  Symbol* s = getSymbol(node->token->value.str);
  //não trato caso de não achar o símbolo na tabela; devia ser pego antes

  insertTailInstruction(jumpI(s->label), &(node->inst_head));
  // addInstToNode(jumpI(s->label), node, false);
  // addInstToList(jumpI(s->label), node->instructions);

  node->regTemp = getRegister();
  insertTailInstruction(loadAI(RSP, 0, node->regTemp), &(node->inst_head));
  // addInstToNode(loadAI(RSP, 0, node->regTemp), node, false);
  // addInstToList(loadAI(RSP, 0, node->regTemp), node->instructions);
  //copia valor de retorno p/ node->regTemp

}

void varLocalCode(Node* node) {

  insertTailInstruction(addI(RSP, inferSizeForType(node->varType, 1), RSP), &(node->inst_head));
  // addInstToNode(addI(RSP, inferSizeForType(node->varType, 1), RSP), node, false);
  // addInstToList(addI(RSP, inferSizeForType(node->varType, 1), RSP), node->instructions);
}

void callArgListCode(Node *node) {
  if (node->kidsNumber > 0) {

    Node *aux = node->kids[node->kidsNumber - 1];

    while (aux != NULL) {
      concatInstructions(&(node->inst_head), &(aux->inst_head));
      // concatInstructions(node, aux, false); //append no final
      // node->instructions = concatLists(node->instructions, aux->instructions);
      //código do argumento da chamada
      insertTailInstruction(storeAI(aux->regTemp, RSP, 0), &(node->inst_head));
      // addInstToNode(storeAI(aux->regTemp, RSP, 0), node, false); //adiciona no final
      // addInstToList(storeAI(aux->regTemp, RSP, 0), node->instructions);
      //guarda resultado em RSP+0
      insertTailInstruction(addI(RSP, 4, RSP), &(node->inst_head));
      // addInstToNode(addI(RSP, 4, RSP), node, false); //adiciona no final
      // addInstToList(addI(RSP, 4, RSP), node->instructions);
      //avança RSP 4 posições

      if (aux->kidsNumber > 0) {
        aux = aux->kids[aux->kidsNumber - 1];
      }
      else {
        aux = NULL;
      }
    }

    aux = NULL;
  }
}

void blockCode(Node *node) {

  if (node->kidsNumber > 0) {

    Node *aux = node->kids[node->kidsNumber - 1];

    while (aux != NULL) {
      concatInstructions(&(node->inst_head), &(aux->inst_head));
      // concatInstructions(node, aux, false);
      // node->instructions = concatLists(node->instructions, aux->instructions);
      if (aux->kidsNumber > 0) {
        aux = aux->kids[aux->kidsNumber - 1];
      }
      else {
        aux = NULL;
      }
    }
  }
}

void iloc_init(Node* node) {
  // addInstToList(loadI(1024, RFP), list);
  // addInstToList(loadI(1024, RSP), list);
  // addInstToList(loadI(0, RBSS), list);
  printf("\nhas_main: %d\n", has_main);
  if(has_main) {
    insertHeadInstruction(jumpI(0), &(node->inst_head));
    // addInstToNode(jumpI(0), node, true); //main é L0; adiciona no topo
    // node->instructions = concatLists(list, node->instructions); //bota essas insts no inicio do programa
  }
  
 	// list = NULL;

  insertTailInstruction(halt(), &(node->inst_head));
  // addInstToNode(halt(), node, false); //adiciona halt no final
  // addInstToList(halt(), node->instructions); //adiciona halt no final
}

//###########################

void remendaReturn(Node* node) {

  IList *aux;

  getNodeByOp(node->inst_head, RETJUMP, &aux);

  if(aux) {
    aux->inst.arg1 = return_label;
  }
}

void remendaTrue(Node *node, int newLabel) {
  int label = -1;
  IList *head = node->inst_head;
  IList *aux;

  for(int i = 0; i < node->trueNmr; i++) {
    head = node->inst_head;
    label = node->tl[i];

    do {
      getNodeByOp(head, CBR, &aux);
      if(aux) {
        if(aux->inst.arg2 == label) {
          aux->inst.arg2 = newLabel;
        }
        head = aux->next;
      } else {
        head = NULL;
      }
    } while (head);
  }
}

//x && y && z && w

void remendaFalse(Node *node, int newLabel) {
  int label = -1;
  IList *head = node->inst_head;
  IList *aux;

  for(int i = 0; i < node->falseNmr; i++) {
    head = node->inst_head;
    label = node->fl[i];

    do {
      getNodeByOp(head, CBR, &aux);
      if(aux) {
        if(aux->inst.arg3 == label) {
          aux->inst.arg3 = newLabel;
        }
        head = aux->next;
      } else {
        head = NULL;
      }
    } while (head);
  }
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

void printCode(Node* node) {

  IList *aux = node->inst_head;

  while (aux) {
    printInstruction(aux->inst);
    aux = aux->next;
  }
}

void printInstruction(Instruction instruction) {
  switch (instruction.op_code) {
  case LBL:
    printf("L%d: nop\n", instruction.arg1);
    break;
  case NOP:
    printf("nop \n");
    break;
  case ADD:
    printf("add r%d, r%d => r%d\n", instruction.arg1, instruction.arg2, instruction.arg3);
    break;
  case ADDI:
    if (instruction.arg1 < 0 && instruction.arg3 < 0) {
      printf("addI %s, %d => %s\n", special_register(instruction.arg1), instruction.arg2, special_register(instruction.arg3));
    } else if (instruction.arg1 < 0) {
      printf("addI %s, %d => r%d\n", special_register(instruction.arg1), instruction.arg2, instruction.arg3);
    } else if (instruction.arg3 < 0) {
      printf("addI r%d, %d => %s\n", instruction.arg1, instruction.arg2, special_register(instruction.arg3));
    } else {
      printf("addI r%d, %d => r%d\n", instruction.arg1, instruction.arg2, instruction.arg3);
    }
    break;
  case SUB:
    printf("sub r%d, r%d => r%d\n",  instruction.arg1, instruction.arg2, instruction.arg3);
    break;
  case SUBI:
    printf("subI r%d, %d => r%d\n", instruction.arg1, instruction.arg2, instruction.arg3);
    break;
  case RSUBI:
    printf("rsubI r%d, %d => r%d\n", instruction.arg1, instruction.arg2, instruction.arg3);
    break;
  case MULT:
    printf("mult r%d, r%d => r%d\n", instruction.arg1, instruction.arg2, instruction.arg3);
    break;
  case MULTI:
    printf("multI r%d, %d => r%d\n", instruction.arg1, instruction.arg2, instruction.arg3);
    break;
  case DIV:
    printf("div r%d, r%d => r%d\n", instruction.arg1, instruction.arg2, instruction.arg3);
    break;
  case DIVI:
    printf("divI r%d, %d => r%d\n", instruction.arg1, instruction.arg2, instruction.arg3);
    break;
  case RDIVI:
    printf("rdivI r%d, %d => r%d\n", instruction.arg1, instruction.arg2, instruction.arg3);
    break;
  case LSHIFT:
    printf("lshift r%d, r%d => r%d\n", instruction.arg1, instruction.arg2, instruction.arg3);
  case RSHIFT:
    printf("rshift r%d, r%d => r%d\n", instruction.arg1, instruction.arg2, instruction.arg3);
  case LOADI: // r2 = c1 | constante
    if(instruction.arg2 < 0)
      printf("loadI %d => %s\n", instruction.arg1, special_register(instruction.arg2));
    else
      printf("loadI %d => r%d\n", instruction.arg1, instruction.arg2);
    break;
  case LOADA0: //só usar especial como 1o arg
    if(instruction.arg1 < 0)
      printf("loadA0 %s, r%d => r%d\n", special_register(instruction.arg1), instruction.arg2, instruction.arg3);
    else
      printf("loadA0 r%d, r%d => r%d\n", instruction.arg1, instruction.arg2, instruction.arg3);
    break;
  case LOADAI: // r3 = Mem(r1 + c2) | constante/offset
    if(instruction.arg1 < 0)
      printf("loadAI %s, %d => r%d\n", special_register(instruction.arg1), instruction.arg2, instruction.arg3);
    else //acho que só é usado com os especiais
      printf("loadAI r%d, %d => r%d\n", instruction.arg1, instruction.arg2, instruction.arg3);
    break;
  case STOREAI: // storeAI r1 => r2, c3 | Mem(r2 + c3) = r1
    if(instruction.arg2 < 0)
      printf("storeAI r%d => %s, %d\n", instruction.arg1, special_register(instruction.arg2), instruction.arg3);
    else
      printf("storeAI r%d => r%d, %d\n", instruction.arg1, instruction.arg2, instruction.arg3);
    break;
  case STOREA0:
    if(instruction.arg2 < 0)
      printf("storeA0 r%d => %s, r%d\n", instruction.arg1, special_register(instruction.arg2), instruction.arg3);
    else
      printf("storeA0 r%d => r%d, r%d\n", instruction.arg1, instruction.arg2, instruction.arg3);
    break;
  case I2I: //i2i r1 => r2 | r1 = r2 para inteiros
    if (instruction.arg1 < 0 && instruction.arg2 < 0) {
      printf("i2i %s => %s\n", special_register(instruction.arg1), special_register(instruction.arg2));
    } else if (instruction.arg1 < 0) {
      printf("i2i %s => r%d\n", special_register(instruction.arg1), instruction.arg2);
    } else if (instruction.arg2 < 0) {
      printf("i2i r%d => %s\n", instruction.arg1, special_register(instruction.arg2));
    } else {
      printf("i2i r%d => r%d\n", instruction.arg1, instruction.arg2);
    }
    break;
  case JUMPI:
  case RETJUMP:
    printf("jumpI -> L%d\n", instruction.arg1);
    break;
  case JUMP:
    printf("jump -> r%d\n", instruction.arg1);
    break;
  case CBR: // cbr r1 -> l2, l3 | r1 ? pc = endereco(l2) : pc=endereco(l3)
    printf("cbr r%d -> L%d, L%d\n", instruction.arg1, instruction.arg2, instruction.arg3);
    break;
  case CMP_LT:
    printf("cmp_LT r%d, r%d -> r%d\n", instruction.arg1, instruction.arg2, instruction.arg3);
    break;
  case CMP_LE:
    printf("cmp_LE r%d, r%d -> r%d\n", instruction.arg1, instruction.arg2, instruction.arg3);
    break;
  case CMP_EQ:
    printf("cmp_EQ r%d, r%d -> r%d\n", instruction.arg1, instruction.arg2, instruction.arg3);
    break;
  case CMP_GE:
    printf("cmp_GE r%d, r%d -> r%d\n", instruction.arg1, instruction.arg2, instruction.arg3);
    break;
  case CMP_GT:
    printf("cmp_GT r%d, r%d -> r%d\n", instruction.arg1, instruction.arg2, instruction.arg3);
    break;
  case CMP_NE:
    printf("cmp_NE r%d, r%d -> r%d\n", instruction.arg1, instruction.arg2, instruction.arg3);
    break;
  case HALT:
    printf("halt\n");
    break;
  default:
    break;
  }
}
