#include "ast.h"

void deletaParam() {

}

void libera(void* arvore) {

}

void exporta(void* arvore) {

}

/*............  CRIA NOS  ............*/

Node* make_node(Type type) {
    Node* n = (Node*) malloc(sizeof(Node));
    n->type = type;
    n->val = malloc(sizeof(union Value));
    n->nextNode = NULL;
    return n;
}

Node* make_int(int value) {
    Node* n = make_node(INT);
    n->val->int_value = value;

    return n;
}

Node* make_float(float value) {
    Node* n = make_node(FLOAT);
    n->val->float_value = value;

    return n;
}

Node* make_bool(bool value) {
    Node* n = make_node(BOOL);
    n->val->bool_value = value;

    return n;
}

Node* make_char(char value) {
    Node* n = make_node(CHAR);
    n->val->char_value = value;

    return n;
}

Node* make_string(char* value) {
    Node* n = make_node(STRING);
    n->val->string_value = value;

    return n;

}

Node* make_un_op(Un_Op_Type type, Node* value) {
    Node* n = make_node(UN_OP);
    n->val->un_op.type = type;
    n->val->un_op.node = value;

    return n;
}

Node* make_bin_op(Bin_Op_Type type, Node* left, Node* right) {
    Node* n = make_node(BIN_OP);
    n->val->bin_op.type = type;
    n->val->bin_op.left = left;
    n->val->bin_op.right = right;

    return n;

}

Node* make_ter_op(Node* cond, Node* left, Node* right) {
    Node* n = make_node(TER_OP);
    n->val->ter_op.cond = cond;
    n->val->ter_op.left = left;
    n->val->ter_op.right = right;

    return n;

}

Node* make_global_var(Type type, bool _static, char* name, int array_size) {
    Node* n = make_node(GLOBAL_VAR_DEC);
    n->val->global_var.name = name;
    n->val->global_var._static = _static;
    n->val->global_var.type = type;
    n->val->global_var.array_size = array_size;

    return n;

}

Node* make_global_func(bool _static, Type type, char* name, ParamValue* param, Node* body) {
    Node* n = make_node(FUNC_DEC);
    n->val->global_func._static = _static;
    n->val->global_func.type = type;
    n->val->global_func.name = name;
    n->val->global_func.param = param;
    n->val->global_func.body = body;

    return n;

}

Node* make_local_var(bool _static, bool _const, Type type, char* name, Node* value) {
    Node* n = make_node(VARIABLE);
    n->val->var_local._static = _static;
    n->val->var_local._const = _const;
    n->val->var_local.type = type;
    n->val->var_local.name = name;
    n->val->var_local.lit_ou_id = value;

    return n;

}

Node* make_identificador(char* name, Node* index) {
    Node* n = make_node(IDENTIFICADOR);
    n->val->identificador.name = name;
    n->val->identificador.index = index;

    return n;

}

Node* make_attrib(Node* left, Node* right) {
    Node* n = make_node(ATRIB);
    n->val->attrib.left = left;
    n->val->attrib.right = right;

    return n;
}

Node* make_if(Node* cond, Node* bloco, Node* else_node) {
    Node* n = make_node(IF);

    n->val->if_op.cond = cond;
    n->val->if_op.bloco = bloco;
    n->val->if_op.else_node = else_node;

    return n;

}

Node* make_for(Node* init, Node* expr, Node* attrib, Node* bloco) {
    Node* n = make_node(FOR);

    n->val->for_op.init = init;
    n->val->for_op.expr = expr;
    n->val->for_op.attrib = attrib;
    n->val->for_op.bloco = bloco;

    return n;

}

Node* make_while(Node* expr, Node* bloco) {
    Node* n = make_node(WHILE_DO);

    n->val->while_do_op.expr = expr;
    n->val->while_do_op.bloco = bloco;

    return n;
}

Node* make_func_call(char* name, Node* arg) {
    Node* n = make_node(FUNC_CALL);

    n->val->func_call.name = name;
    n->val->func_call.arg = arg;

    return n;

}

Node* make_input(Node* value) {
    Node* n = make_node(INPUT);
    
    n->val->input.val = value;

    return n;

}

Node* make_output(Node* value) {
    Node* n = make_node(OUTPUT);

    n->val->output.val = value;

    return n;

}

Node* make_return(Node* value) {
    Node* n = make_node(RETURN);

    n->val->retorno.val = value;

    return n;

}

Node* make_block(Node* value) {
    Node* n = make_node(BLOCK);

    n->val->block.val = value;

    return n;

}

Node* make_continue() {
    Node* n = make_node(CONTINUE);

    return n;

}

Node* make_break() {
    Node* n = make_node(BREAK);

    return n;

}


