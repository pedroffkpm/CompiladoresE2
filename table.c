#include "table.h"

SymbolTable* currentScope = NULL;

int inferSizeForType(Type type, int elem_number) {
    switch (type)
    {
    case INT_TYPE:
        return 4 * elem_number;
        break;
    case FLOAT_TYPE:
        return 8 * elem_number;
        break;
    case CHAR_TYPE:
    case STRING_TYPE:
    case BOOL_TYPE:
        return 1 * elem_number;
        break;
    
    default:
        break;
    }

