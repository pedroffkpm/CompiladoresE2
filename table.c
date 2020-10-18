#include "table.h"
#include <string.h>

SymbolTable* currentScope = NULL;

int hashFunction(char *key) {
   unsigned long hash = 5381;
    int c;

    while (c = *key++)
        hash = ((hash << 5) + hash) + c;
    return (int)(hash%HASH_SIZE);
}

int inferSizeForType(Type type, int elem_number) {
    switch (type) {
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
}

SymbolTable *createTable() {
    SymbolTable *new = (SymbolTable *)malloc(sizeof(SymbolTable));

    new->elements = (Symbol **)malloc(sizeof(Symbol *) * HASH_SIZE);

    for (int i = 0; i < HASH_SIZE; i++)
    {
        new->elements[i] = NULL;
    }

    new->nextTable = NULL;

    return new;
}

void pushTable() {

    SymbolTable *new = createTable();

    new->nextTable = currentScope;

    currentScope = new;

    new = NULL;
}

void freeParam(Param *parametro) {
    if (parametro == NULL) {
        return;
    }

    if (parametro->name != NULL) {
        free(parametro->name);
    }

    free(parametro);
}

void popTable() {
    if (currentScope->elements != NULL) {
        for (int i = 0; i < HASH_SIZE; i++) {
            Symbol *elm = currentScope->elements[i];
            for (int j = 0; j < elm->n_params; j++) {
                freeParam(elm->params[j]);
            }

            free(elm->key);
            elm->key = NULL;

            free(elm->valor_lexico);
            elm->valor_lexico = NULL;

            free(elm);
            elm = NULL;

            currentScope->elements[i] = NULL;
        }

        currentScope = currentScope->nextTable;
    }
}

char* boolString(bool b) {
    return b ? "true" : "false";
}

char* makeKey(int keysize, Nature nature, struct lexval *valor_lexico) {
    char* key = malloc((sizeof(char)) * keysize);

    switch (nature)
    {
    case LIT_INT:
        snprintf(key, keysize, "%d", valor_lexico->value.i);
        break;
    case LIT_FLOAT:
        snprintf(key, keysize, "%f", valor_lexico->value.f);
        break;
    case LIT_BOOL:
        snprintf(key, keysize, "%s", boolString(valor_lexico->value.b));
        break;
    case LIT_CHAR:
        snprintf(key, keysize, "%c", valor_lexico->value.c);
        break;
    case VAR:
    case FUNCTION:
    case LIT_STRING:
        snprintf(key, keysize, "%s", valor_lexico->value.str);
        break;
    default:
        break;
    }

    return key;
}

void addSymbol(Nature nature, Type type, int vecSize, int paramSize, Param **params, struct lexval *valor_lexico) {

    int keysize = 50;
    char* key = makeKey(keysize, nature, valor_lexico);

    int index = hashFunction(key);

    while(currentScope->elements[index] != NULL) {
        ++index;
        index %= HASH_SIZE; 
    }

    currentScope->elements[index] = (Symbol*) malloc(sizeof(Symbol));
    currentScope->elements[index]->key = (char*) malloc(sizeof(char) * keysize);
    snprintf(currentScope->elements[index]->key, keysize, "%s", key);
    free(key);

    currentScope->elements[index]->line = valor_lexico->lineNumber;
    currentScope->elements[index]->nature = valor_lexico->nature;
    currentScope->elements[index]->n_params = paramSize;
    currentScope->elements[index]->params = (Param**) malloc(sizeof(Param*) * paramSize);
    memcpy(currentScope->elements[index]->params, params, sizeof(Param*) * paramSize);
    currentScope->elements[index]->type = type;
    currentScope->elements[index]->size = inferSizeForType(type, vecSize);
    currentScope->elements[index]->valor_lexico = (struct lexval*) malloc(sizeof(struct lexval));
    memcpy(currentScope->elements[index]->valor_lexico, valor_lexico, sizeof(struct lexval));

}

Symbol* getSymbol(char *key) {

    int index = hashFunction(key);
    SymbolTable *table = currentScope;
    Symbol *symbol = NULL;

    while (table != NULL) {
        while (table->elements[index] != NULL) {
            if (strncmp(table->elements[index]->key, key, strlen(key)) == 0) {
                symbol = table->elements[index];
                break;
            }
            else {
                ++index;
                index %= HASH_SIZE;
            }
        }

        if(symbol == NULL) {
            table = table->nextTable;
        }
        else {
            return symbol;
        }
    }
}

int inferType(Type left, Type right) {
    if(left == STRING_TYPE || left == CHAR_TYPE || right == STRING_TYPE || right == CHAR_TYPE) {
        return -1;
    }
    if(left == FLOAT_TYPE || right == FLOAT_TYPE) {
        return FLOAT_TYPE;
    }
    if(left == INT_TYPE || right == INT_TYPE) {
        return INT_TYPE;
    }
    if(left == BOOL_TYPE || right == BOOL_TYPE) {
        return BOOL_TYPE;
    }

    return -1;
}

//posso converter int para bool? -> convertType(int, bool)? -> return bool
int convertType(Type from, Type to) { 
    if(inferType(from, to) != -1) {
        return to;
    }
    else {
        return -1;
    }
}

