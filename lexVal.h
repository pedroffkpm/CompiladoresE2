#ifndef LEXVAL_H
#define LEXVAL_H 

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

/*******TIPOS DE TOKENS*******/
#define KEYWORD 0x0
#define SPEC_CHAR 0x1
#define COMP_OPER 0x2
#define IDS 0x3
#define LITERAL 0x4
#define VEC_INDEX 0x5
#define F_CALL 0x6
#define TERN_OP 0x7
/*******TIPOS DE LITERAIS*****/
#define INT 0X8
#define FLOAT 0x9
#define CHAR 0xA
#define BOOL 0XB
#define STRING 0XC

/*Valor NONE é usado no campo para tipo do literal quando o token não é literal*/
#define NONE 0xF
#define REMOVE 0xE


//valores booleanos
#define FALSE 0
#define TRUE 1

//tipo e natureza de um token
typedef enum {
    LIT_INT,
    LIT_FLOAT,
    LIT_CHAR,
    LIT_STRING,
    LIT_BOOL,
    VAR,
    VAR_DEC,
    VECTOR,
    VECTOR_DEC,
    FUNCTION,
    FUNCTION_DEC,
	RETURN,
	UN_OP,
	BIN_OP,
	TER_OP,
	INPUT,
	OUTPUT,
	VEC_ID,
	VEC_IND,
	FUNC_ARG,
	INIT_BLOCK,
	END_BLOCK,
	SHIFT,
	ATTRIB,
	INIT,
	EMPTY

} Nature;

typedef enum {
	ERR_TYPE = -1,
    INT_TYPE,
    FLOAT_TYPE,
    CHAR_TYPE,
    STRING_TYPE,
    BOOL_TYPE
} Type;


//Valores dos tokens
union Value{
	int i;
	float f;
	char c;
	bool b;
	char* str; 
};
// Estrutura usada para associar valor ao token
struct lexval{
	int tokenInAst;
	int lineNumber; 
	int tokenType; 
	int literalType;	
	union Value value; 
};


#endif //LEXVAL_H
