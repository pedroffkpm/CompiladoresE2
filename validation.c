#include "validation.h"

void printError(int errorcode, Node* node, Symbol* symbol) {
    printf("Line %d:\nERROR\t %s: %s\n", lineNumber, errorMessage(errorcode), str);
}

char* errorMessage(int errorcode) {
    switch (errorcode)
    {
    case ERR_UNDECLARED:
        return "Undeclared Identifier";
        break;
    case ERR_DECLARED:
        return "Identifier was already declared";
        break;
    case ERR_VARIABLE:
        return "Variable, but expected vector or function";
        break;
    case ERR_VECTOR:
        return "Vector, but expected variable or function";
        break;
    case ERR_FUNCTION:
        return "Function, but expected vector or variable";
        break;
    case ERR_WRONG_TYPE:
        return "";
            break;
    case ERR_STRING_TO_X:
            break;
    case ERR_CHAR_TO_X:
            break;
    case ERR_STRING_SIZE:
            break;
    case ERR_MISSING_ARGS:
            break;
    case ERR_EXCESS_ARGS:
            break;
    case ERR_WRONG_TYPE_ARGS:
            break;
    case ERR_WRONG_PAR_INPUT:
            break;
    case ERR_WRONG_PAR_OUTPUT:
            break;
    case ERR_WRONG_PAR_RETURN:
            break;
    case ERR_WRONG_PAR_SHIFT:
            break;
    default:
        break;
    }
}

void validateProgram(void* arvore) {
    Node* node = (Node*) arvore;

    SymbolTable* scope = createTable();

    validateTable(scope, node); //já printa erro? ou retorna string?

    deleteTables();

    free(scope);

}

void validateVar(Node* node) {
    Symbol* s = getSymbol(node->token->value.str);
    if (s == NULL) {
        printError(ERR_UNDECLARED, node, s);
        exit(ERR_UNDECLARED);
    }


}

void validateTable(SymbolTable* table, Node* node) {
    if(node == NULL) {
        return;
    }

    switch (node->token->nature)
    {
    case LIT_INT:
        addSymbol(LIT_INT, node->token->varType, 1, 0, NULL, node->token);
        break;
    case LIT_FLOAT:
        addSymbol(LIT_FLOAT, node->token->varType, 1, 0, NULL,node->token);
        break;
    case LIT_CHAR:
        addSymbol(LIT_CHAR, node->token->varType, 1, 0, NULL,node->token);
        break;
    case LIT_STRING:
        addSymbol(LIT_STRING, node->token->varType, 1, 0, NULL,node->token);
        break;
    case LIT_BOOL:
        addSymbol(LIT_BOOL, node->token->varType, 1, 0, NULL,node->token);
        break;
    case VAR:
        addSymbol(VAR, node->token->varType, 1, 0, NULL,node->token);
        break;
    case VECTOR:
        addSymbol(LIT_FLOAT, node->token->varType, 1, 0, NULL,node->token);
        break;
    case FUNCTION:
        break;
	case RETURN:
        break;
	case UN_OP:
        break;
	case BIN_OP:
        break;
	case TER_OP:
        break;
	case INPUT:
        break;
	case OUTPUT:
        break;
	case VEC_ID:
        break;
	case VEC_IND:
        break;
    
    default:
        break;
    }

}
