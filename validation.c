#include "validation.h"

char* typeToString(Type t) {
    switch (t)
    {
    case INT_TYPE:
        return "int";
        break;
    case FLOAT_TYPE:
        return "float";
        break;
    case CHAR_TYPE:
        return "char";
        break;
    case STRING_TYPE:
        return "string";
        break;
    case BOOL_TYPE:
        return "bool";
        break;
    default:
        return "";
        break;
    }
}

char* errorMessage(int errorcode) {
    switch (errorcode)
    {
    case ERR_UNDECLARED: //SIMPLE ERROR
        return "Undeclared Identifier";
        break;
    case ERR_DECLARED: //SIMPLE ERROR
        return "Identifier was already declared";
        break;
    case ERR_VARIABLE: //SIMPLE ERROR
        return "Variable, but expected vector or function";
        break;
    case ERR_VECTOR: //SIMPLE ERROR
        return "Vector, but expected variable or function";
        break;
    case ERR_FUNCTION: //SIMPLE ERROR
        return "Function, but expected vector or variable";
        break;
    case ERR_MISSING_ARGS: //SIMPLE ERROR
        return "Missing Args for function";
        break;
    case ERR_EXCESS_ARGS: //SIMPLE ERROR
        return "Too many Args for function";
        break;
    case ERR_WRONG_TYPE: //TYPE ERROR
        return "Wrong Type"; //deve vir uma msg "expected X but got Y" depois
        break;
    case ERR_WRONG_TYPE_ARGS: //TYPE ERROR
        return "Argument is of wrong type";
        break;
    case ERR_WRONG_PAR_INPUT: //TYPE ERROR
        return "Type not supported for input";
        break;
    case ERR_WRONG_PAR_OUTPUT: //TYPE ERROR
        return "Type not supported for output";
        break;
    case ERR_WRONG_PAR_RETURN: //TYPE ERROR
        return "Wrong type for return"; //opctionalmente: "expected x, but got y"
        break;
    case ERR_STRING_TO_X: //CONVERSION ERROR
        return "Cannot convert String to type"; //deve vir segundo tipo depois
        break;
    case ERR_CHAR_TO_X: //CONVERSION ERROR
        return "Cannot convert Char to type"; //deve vir segundo tipo depois
        break;
    case ERR_STRING_SIZE: //SIZE ERROR
        return "Incompatible size for String";
        break;
    case ERR_WRONG_PAR_SHIFT: //SHIFT ERROR
        return "Wrong value for shift";
        break;
    default:
        return "";
        break;
    }
}

void printSimpleError(int errorcode, int lineNumber, char* name) {
    printf("Line %d:on %s\n\tERROR\t %s\n", lineNumber, name, errorMessage(errorcode));
}

void printTypeError(int errorcode, int lineNumber, Type expected, Type real, char* name) {
    printf("Line: %d: on %s\n\tERROR\t %s: expected %s but got %s\n", lineNumber, name, errorMessage(errorcode), typeToString(expected), typeToString(real));
}

void printConversionError(int errorcode, int lineNumber, Type wrong, char* name) {
    printf("Line: %d: on %s\n\tERROR\t %s %s\n", lineNumber, name, errorMessage(errorcode), typeToString(wrong));
}

void printSizeError(int errorcode, int lineNumber, int expected, int real, char* name) {
    printf("Line: %d: on %s\n\tERROR\t %s: expected %d but got %d\n", lineNumber, name, errorMessage(errorcode), expected, real);
}

void printShiftError(int errorcode, int lineNumber, int passedNumber, char* name) {
    printf("Line: %d: on %s\n\tERROR\t %s: expected max 16, but got %d\n", lineNumber, name, errorMessage(errorcode), passedNumber);
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
        printSimpleError(ERR_UNDECLARED, node->token->lineNumber, node->token->value.str);
        exit(ERR_UNDECLARED);
    }

    if(s->nature == FUNCTION) {
        printSimpleError(ERR_FUNCTION, node->token->lineNumber, node->token->value.str);
        exit(ERR_FUNCTION);
    }

    if(s->nature ) {}


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
