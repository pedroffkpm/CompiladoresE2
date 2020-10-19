#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "ast.h"

#define HASH_SIZE 9973

// typedef enum {
//     LIT_INT,
//     LIT_FLOAT,
//     LIT_CHAR,
//     LIT_STRING,
//     LIT_BOOL,
//     VAR,
//     FUNCTION,

// } Nature;

// typedef enum {
//     INT_TYPE,
//     FLOAT_TYPE,
//     CHAR_TYPE,
//     STRING_TYPE,
//     BOOL_TYPE
// } Type;

typedef struct Param Param;

typedef struct Param
{
    Type type;
    char* name;
    bool _const;
	Param *next;
} Param;


typedef struct Symbol {
    char* key;
    int line;
    Nature nature;
    Type type;
    int size;
    int n_params;
    Param* params;
    struct lexval* valor_lexico;
} Symbol;

typedef struct SymbolTable {
    Symbol** elements;
    struct SymbolTable* nextTable;

} SymbolTable;

Param* createParam(Node *node);
SymbolTable* createTable();
void deleteTables();

void pushTable();
void popTable();

void addSymbol(Nature nature, Type type, int vecSize, Param* params, struct lexval* valor_lexico);

Symbol* getSymbol(char* key);

Symbol* getSymbolOnTable(char *key);

int inferSizeForType(Type type, int elem_number);

int inferType(Type left, Type right);

int convertType(Type from, Type to);
