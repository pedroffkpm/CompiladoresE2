#include "ast.h"

void libera(Node* node) {
    if(node == NULL) {
        return;
    }

    switch (node->type)
    {
    case INT:
    case FLOAT:
    case BOOL:
    case CHAR:
        break;
    case STRING:
        free(node->val->string_value);
        break;
    case IDENTIFICADOR:
        free(node->val->identificador.name);
        libera(node->val->identificador.index);
        break;
    case UN_OP:
        libera(node->val->un_op.node);
        break;
    case BIN_OP:
        libera(node->val->bin_op.left);
        libera(node->val->bin_op.right);
        break;
    case TERN_OP:
        libera(node->val->tern_op.cond);
        libera(node->val->tern_op.left);
        libera(node->val->tern_op.right);
        break;
    case VARIABLE:
        free(node->val->var_local.identificador);
        libera(node->val->var_local.init);
        break;
    case GLOBAL_VAR_DEC:
        free(node->val->global_var.name);
        break;
    case FUNC_DEC:
        free(node->val->global_func.name);
        libera(node->val->global_func.body);
        //o que fazer com param? eh memory leak?
        liberaParam(node->val->global_func.param);
        break;
    case BLOCK:
        libera(node->val->block.val);
        break;
    case ATRIB:
        liberaVariable(node->val->attrib.left);
        libera(node->val->attrib.right);
        break;
    case FUNC_CALL:
        free(node->val->func_call.name);
        libera(node->val->func_call.arg);
        break;
    case RETURN:
        libera(node->val->retorno.val);
        break;
    case BREAK:
    case CONTINUE:
        break;
    case INPUT:
        libera(node->val->input.val);
        break;
    case OUTPUT:
        libera(node->val->output.val);
        break;
    case IF:
        libera(node->val->if_op.cond);
        libera(node->val->if_op.then);
        libera(node->val->if_op.else_node);
        break;
    case FOR:
        libera(node->val->for_op.init);
        libera(node->val->for_op.expr);
        libera(node->val->for_op.attrib);
        libera(node->val->for_op.bloco);
        break;
    case WHILE_DO:
        libera(node->val->while_do_op.expr);
        libera(node->val->while_do_op.bloco);
        break;
    default:
        break;
    }

    if(node->nextNode != NULL) {
        libera(node->nextNode);
    }

    free(node->val);
    free(node);

}

void liberaParam(ParamNode* pnode) {
    if(pnode == NULL) {
        return;
    }

    if(pnode->name != NULL) {
        free(pnode->name);
    }

    if(pnode->nextNode != NULL) {
        liberaParam(pnode->nextNode);
    }

    free(pnode);
}

void liberaVariable(Variable* idnode) {
    if(idnode == NULL) {
        return;
    }

    if(idnode->name != NULL) {
        free(idnode->name);
    }

    libera(idnode->index);
    free(idnode);
}

void exporta(Node* node) {
    if (node == NULL) {
        return;
    }
    print_tree(node);
    return;
}

void print_param(ParamNode* node) {
    if(node == NULL) {
        return;
    }

    if(node->nextNode != NULL) {
        printf("%p, %p\n", node, node->nextNode);
    }

    print_param(node->nextNode);
    return;
}

void print_tree(Node* node) {

    if(node == NULL) {
        return;
    }

    switch (node->type)
    {
    case INT:
    case FLOAT:
    case BOOL:
    case CHAR:
    case STRING:
    case IDENTIFICADOR:
        break;
    case UN_OP:
        printf("%p, %p\n", node, node->val->un_op.node);
        print_tree(node->val->un_op.node);
        break;
    case BIN_OP:
        printf("%p, %p\n", node, node->val->bin_op.left);
        printf("%p, %p\n", node, node->val->bin_op.right);
        print_tree(node->val->bin_op.left);
        print_tree(node->val->bin_op.right);
        break;
    case TERN_OP:
        printf("%p, %p\n", node, node->val->tern_op.cond);
        printf("%p, %p\n", node, node->val->tern_op.left);
        printf("%p, %p\n", node, node->val->tern_op.right);
        print_tree(node->val->tern_op.cond);
        print_tree(node->val->tern_op.left);
        print_tree(node->val->tern_op.right);
        break;
    case VARIABLE:
        printf("%p, %p\n", node, node->val->var_local.init);
        print_tree(node->val->var_local.init);
        break;
    case GLOBAL_VAR_DEC:
        break;
    case FUNC_DEC:
        printf("%p, %p\n", node, node->val->global_func.body);
        printf("%p, %p\n", node, node->val->global_func.param);
        print_param(node->val->global_func.param);
        print_tree(node->val->global_func.body); //aqui
        break;
    case BLOCK:
        break;
    case ATRIB:
        printf("%p, %p\n", node, node->val->attrib.left);
        printf("%p, %p\n", node, node->val->attrib.right);

        print_tree(node->val->attrib.right);
        break;
    case FUNC_CALL:
        printf("%p, %p\n", node, node->val->func_call.arg);
        print_tree(node->val->func_call.arg);
        break;
    case RETURN:
        printf("%p, %p\n", node, node->val->retorno.val);
        print_tree(node->val->retorno.val);
        break;
    case BREAK:
    case CONTINUE:
        break;
    case INPUT:
        printf("%p, %p\n", node, node->val->input.val);
        print_tree(node->val->input.val);
        break;
    case OUTPUT:
        printf("%p, %p\n", node, node->val->output.val);
        print_tree(node->val->output.val);
        break;
    case IF:
        print_tree(node->val->if_op.cond);
        print_tree(node->val->if_op.then);
        print_tree(node->val->if_op.else_node);
        break;
    case FOR:
        print_tree(node->val->for_op.init);
        print_tree(node->val->for_op.expr);
        print_tree(node->val->for_op.attrib);
        print_tree(node->val->for_op.bloco);
        break;
    case WHILE_DO:
        print_tree(node->val->while_do_op.expr);
        print_tree(node->val->while_do_op.bloco);
        break;
    default:
        break;
    }

    print_tree(node->nextNode);
    return;
}



void print_label(char* label) {
    printf(" [label=\"%s\"];\n", label);
}

void print_un_op(Un_Op_Type type) {
    switch(type) {
        case PLUS:
            print_label("+");
            break;
        case MINUS:
            print_label("-");
            break;
        case NOT:
            print_label("!");
            break;
        case ADDRESS:
            print_label("&");
            break;
        case POINTER_VALUE:
            print_label("*");
            break;
        case QUESTION_MARK:
            print_label("?");
            break;
        case HASH:
            print_label("#");
            break;
        default:
            break;
    }
    return;
}

void print_bin_op(Bin_Op_Type type) {
    switch(type) {
        case ADD:
            print_label("+");
            break;
        case SUB:
            print_label("-");
            break;
        case MUL:
            print_label("*");
            break;
        case DIV:
            print_label("/");
            break;
        case DIV_REST:
            print_label("%%");
            break;
        case BIT_OR:
            print_label("|");
            break;
        case BIT_AND:
            print_label("&");
            break;
        case POW:
            print_label("^");
            break;
        case LESS_EQ:
            print_label("<=");
            break;
        case GREATER_EQ:
            print_label(">=");
            break;
        case EQUAL:
            print_label("==");
            break;
        case N_EQUAL:
            print_label("!=");
            break;
        case AND:
            print_label("&&");
            break;
        case OR:
            print_label("||");
            break;
        case SHIFT_L:
        print_label("<<");
        break;
        case SHIFT_R:
            print_label(">>");
            break;
        case BASH_PIPE:
            print_label("|");
            break;
        case FORWARD_PIPE:
            print_label(">");
    }
    return;
}

void print_nome(Node* node) {
    if (node == NULL) {
        return;
    }

    if (node->val != NULL) {
    printf("%p", node);
    }

    switch (node->type)
    {
    case INT:
        char label[12];
        sprintf(label, "%d", node->val->int_value);
        print_label(label);
        break;
    case FLOAT:
        char label[25];
        sprintf(label, "%f", node->val->float_value);
        print_label(label);
        break;
    case BOOL:
        print_label(node->val->bool_value ? "true" : "false");
        break;
    case CHAR:
        printf(" [label=\"%c\"];\n", node->val->char_value);
        break;
    case STRING:
        print_label(node->val->string_value);
        break;
    case IDENTIFICADOR:
        print_label(node->val->identificador.name);
        break;
    case UN_OP:
        print_un_op(node->val->un_op.type);
        break;
    case BIN_OP:
        print_bin_op(node->val->bin_op.type);
        break;
    case TERN_OP:
        print_label("?:");
        break;
    case VARIABLE:
        if(node->val->var_local.init != NULL) {
            print_label("<=");
        }
        break;
    case GLOBAL_VAR_DEC:
        print_label(node->val->global_var.name);
        break;
    case FUNC_DEC:
        print_label(node->val->global_func.name);
        break;
    case BLOCK:
        break;
    case ATRIB:
        print_label("=");
        break;
    case FUNC_CALL:
        printf(" [label=\"call %s\"];\n", node->val->func_call.name);
        break;
    case RETURN:
        print_label("return");
        break;
    case BREAK:
        print_label("break");
        break;
    case CONTINUE:
        print_label("continue");
        break;
    case INPUT:
        print_label("input");
        break;
    case OUTPUT:
        print_label("output");
        break;
    case IF:
        print_label("if");
        break;
    case FOR:
        print_label("for");
        break;
    case WHILE_DO:
        print_label("while");
        break;
    default:
        break;
    }


}

/*............  CRIA NOS  ............*/

Node* make_node(Type type) {
    Node* n = (Node*) malloc(sizeof(Node));
    n->type = type;
    n->val = malloc(sizeof(union Value));
    n->nextNode = NULL;
    return n;
}

ParamNode* make_param(char* name) {
    ParamNode* n = (ParamNode*) malloc(sizeof(ParamNode));
    n->name = name;
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

Node* make_tern_op(Node* cond, Node* left, Node* right) {
    Node* n = make_node(TERN_OP);
    n->val->tern_op.cond = cond;
    n->val->tern_op.left = left;
    n->val->tern_op.right = right;

    return n;
}

Node* make_global_var(char* name, int array_size) {
    Node* n = make_node(GLOBAL_VAR_DEC);
    n->val->global_var.name = name;
    n->val->global_var.array_size = array_size;

    return n;

}

Node* make_global_func(char* name, ParamNode* param, Node* body) {
    Node* n = make_node(FUNC_DEC);
    n->val->global_func.name = name;
    n->val->global_func.param = param;
    n->val->global_func.body = body;

    return n;

}

Node* make_local_var(char* name, Node* value) {
    Node* n = make_node(VARIABLE);
    n->val->var_local.identificador = name;
    n->val->var_local.init = value;

    return n;

}

Node* make_identificador(char* name, Node* index) {
    Node* n = make_node(IDENTIFICADOR);
    n->val->identificador.name = name;
    n->val->identificador.index = index;

    return n;

}

Node* make_attrib(Node* left, Node* right) {
    Variable old_left = left->val->identificador;
    Variable* new_left = (Variable*) malloc(sizeof(Variable));

    new_left->name = old_left.name;
    new_left->index = old_left.index;

    Node* n = make_node(ATRIB);
    n->val->attrib.left = new_left;
    n->val->attrib.right = right;

    old_left.index = NULL;
    old_left.name = NULL;
    free(left->val);
    free(left);

    return n;
}

Node* make_if(Node* cond, Node* then, Node* else_node) {
    Node* n = make_node(IF);

    n->val->if_op.cond = cond;
    n->val->if_op.then = then;
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


