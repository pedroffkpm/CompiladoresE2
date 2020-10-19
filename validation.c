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

    if(s->nature == VECTOR) {
		printSimpleError(ERR_VECTOR, node->token->lineNumber, node->token->value.str);
        exit(ERR_VECTOR);		
	}
	return;
}

void validateVector(Node* node) {
    Symbol* s = getSymbol(node->token->value.str);
    if (s == NULL) {
        printSimpleError(ERR_UNDECLARED, node->token->lineNumber, node->token->value.str);
        exit(ERR_UNDECLARED);
    }

    if(s->nature == FUNCTION) {
        printSimpleError(ERR_FUNCTION, node->token->lineNumber, node->token->value.str);
        exit(ERR_FUNCTION);
    }

    if(s->nature == VAR) {
		printSimpleError(ERR_VARIABLE, node->token->lineNumber, node->token->value.str);
        exit(ERR_VARIABLE);		
	}
	return;
}

void validateFunctionArgs(Node *node, Param *param) {
	if (param == NULL){
		if (node == NULL) {
			return;
		} else {
			printSimpleError(ERR_EXCESS_ARGS, node->token->lineNumber, node->token->value.str);
			exit(ERR_EXCESS_ARGS);
		}
	} else if(node == NULL) {
		printSimpleError(ERR_MISSING_ARGS, 0, param->name);
		exit(ERR_MISSING_ARGS);
	}
	if(param->type == node->token->varType) {
		validateFunctionArgs(node->kids[0], param->next);
	} else {
		printTypeError(ERR_WRONG_TYPE_ARGS, node->token->lineNumber, param->type, node->token->varType, node->token->value.str);
		exit(ERR_WRONG_TYPE_ARGS);
	}
	return;
	
}


void validateFunction(Node* node) {
    Symbol* s = getSymbol(node->token->value.str);
    if (s == NULL) {
        printSimpleError(ERR_UNDECLARED, node->token->lineNumber, node->token->value.str);
        exit(ERR_UNDECLARED);
    }

    if(s->nature == VAR) {
        printSimpleError(ERR_VARIABLE, node->token->lineNumber, node->token->value.str);
        exit(ERR_VARIABLE);
    }

    if(s->nature == VECTOR) {
		printSimpleError(ERR_VECTOR, node->token->lineNumber, node->token->value.str);
        exit(ERR_VECTOR);		
	}
	validateFunctionArgs(node->kids[0], s->params);
	return;
}
/*
void validateUnOp(Node *node){
	if(node->kids[0]->token->nature == VAR) {
		Symbol* s = getSymbol(node->token->value.str);
		validateVar(node->kids[0]);
	}
	if(node->kids[0]->token->nature == VECTOR) {
		Symbol* s = getSymbol(node->token->value.str);
		validateVector(node->kids[0]);
	}
	return;
}
*/

void validateBinOp(Node *node) {
	if(node->token->varType == -1) {
		printSimpleError(ERR_STRING_TO_X, node->kids[0]->token->lineNumber, NULL); //o q printar
		exit(ERR_STRING_TO_X);
	}/*
	Symbol* s = getSymbolOnTable(node->token->value.str);
	if(node->token->varType == STRING_TYPE) {
		
	}*/
	return;
}

void isDeclared(Node* node) {
	Symbol* s = getSymbolOnTable(node->token->value.str);
    if (s == NULL) {
        return;
    }
	printSimpleError(ERR_DECLARED, node->token->lineNumber, node->token->value.str);
    exit(ERR_DECLARED);
}

void validateInput(Node *node) {
	Symbol* s = getSymbol(node->kids[0]->token->value.str);
	if(s->type == INT && s->type == FLOAT) {
		return;
	}
	printTypeError(ERR_WRONG_PAR_INPUT, node->token->lineNumber, INT_TYPE, s->type, node->kids[0]->token->value.str);	
	return;
}

void validateOutput(Node *node) {
	Symbol* s = getSymbol(node->kids[0]->token->value.str);
	if(s->type == INT && s->type == FLOAT) {
		return;
	}
	printTypeError(ERR_WRONG_PAR_OUTPUT, node->token->lineNumber, INT_TYPE, s->type, node->kids[0]->token->value.str);	
	return;
}

void validateReturn(Node *node) {
	return;
}

void validateShift(Node *node) {
	if(node->kids[1]->token->value.i > 16) {
		printShiftError(ERR_WRONG_PAR_SHIFT, node->token->lineNumber, node->kids[1]->token->value.i, node->kids[2]->token->value.str);
		exit(ERR_WRONG_PAR_SHIFT);
	}
	return;
}

void validateAttribution(Node *node) {
	Symbol* s = getSymbol(node->kids[0]->token->value.str);
	if(s->type == node->kids[1]->token->varType) {
		if(s->type == STRING_TYPE) {
			if(s->size != sizeof(node->kids[1]->token->value.str)) {
				printSizeError(ERR_STRING_SIZE, node->token->lineNumber, sizeof(node->kids[1]->token->value.str), s->size, s->key);
				exit(ERR_STRING_SIZE);
			}
		}
		return;
	}
	if(node->kids[1]->token->varType == CHAR_TYPE){
		printConversionError(ERR_CHAR_TO_X, node->token->lineNumber, s->type, node->kids[0]->token->value.str);
		exit(ERR_CHAR_TO_X);
	}
	if(node->kids[1]->token->varType == STRING_TYPE){
		printConversionError(ERR_STRING_TO_X, node->token->lineNumber, s->type, node->kids[0]->token->value.str);
		exit(ERR_STRING_TO_X);
	}
	return;
}


void validateTable(SymbolTable* table, Node* node) {
    if(node == NULL) {
        return;
    }
    switch (node->token->nature)
    {
    case LIT_INT:
        addSymbol(LIT_INT, node->token->varType, 1, NULL, node->token);
        break;
    case LIT_FLOAT:
        addSymbol(LIT_FLOAT, node->token->varType, 1, NULL,node->token);
        break;
    case LIT_CHAR:
        addSymbol(LIT_CHAR, node->token->varType, 1, NULL,node->token);
        break;
    case LIT_STRING:
        addSymbol(LIT_STRING, node->token->varType, 1, NULL,node->token);
        break;
    case LIT_BOOL:
        addSymbol(LIT_BOOL, node->token->varType, 1, NULL,node->token);
        break;
	case VAR:
		validateVar(node);
		break;
    case VAR_DEC:
		isDeclared(node);
        addSymbol(VAR, node->token->varType, 1, NULL,node->token);
        break;
	case VECTOR:
		validateVector(node);
    case VECTOR_DEC:
		isDeclared(node);	
        addSymbol(node->token->nature, node->kids[0]->token->varType, node->kids[1]->token->value.i, NULL, node->kids[0]->token);
        break;
	case FUNCTION:
		validateFunction(node);
    case FUNCTION_DEC:
		isDeclared(node);
		addSymbol(FUNCTION, node->token->varType, 1, createParam(node->kids[0]), node->token); //push
        break;
	case RETURN:
		validateReturn(node); //falta
        break;
	case UN_OP:
        break;
	case BIN_OP:
		validateBinOp(node);
        break;
	case TER_OP: //falta
        break;
	case INPUT:
		validateInput(node); 
        break;
	case OUTPUT:
		validateOutput(node);
        break;
	case VEC_ID: //ignore
        break;
	case VEC_IND: //ignore
        break;
	case FUNC_ARG: //ignore
		break;
	case INIT_BLOCK:
		pushTable();
		break;
	case END_BLOCK:
		popTable();
		break;
	case SHIFT:
		validateShift(node);
		break;
	case ATTRIB:
		validateAttribution(node);
		break;
	case INIT:
		break;
    default:
        break;
    }
	return;
}
