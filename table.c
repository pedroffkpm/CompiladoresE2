#include "table.h"

SymbolTable* currentScope = NULL;

int hashFunction(char *key) {
    //to do

    return 5;
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

void addSymbol(Nature nature, Type type, Param **params, struct lexval *valor_lexico) {
    //?
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