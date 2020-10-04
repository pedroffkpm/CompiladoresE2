%{
    #include <stdio.h>
    #include <stdlib.h>
    #include "lex.yy.h"
    
    #define YYERROR_VERBOSE 1

    extern void* arvore;

    extern int get_line_number(void);
    extern int get_column_number(void);
    int yylex(void);
    int yyerror (char const *s);
%}

%code requires {
    #include "ast.h"
}



%union {
    struct valor_lexico_t {
        int line;
        int col;
        TokenType tipo;
        TokenValue valor;
    } valor_lexico;

    // Node* node;
    // ParamNode* param;

}

%define parse.error verbose

%token TK_PR_INT
%token TK_PR_FLOAT
%token TK_PR_BOOL
%token TK_PR_CHAR
%token TK_PR_STRING
%token TK_PR_IF
%token TK_PR_THEN
%token TK_PR_ELSE
%token TK_PR_WHILE
%token TK_PR_DO
%token TK_PR_INPUT
%token TK_PR_OUTPUT
%token TK_PR_RETURN
%token TK_PR_CONST
%token TK_PR_STATIC
%token TK_PR_FOREACH
%token TK_PR_FOR
%token TK_PR_SWITCH
%token TK_PR_CASE
%token TK_PR_BREAK
%token TK_PR_CONTINUE
%token TK_PR_CLASS
%token TK_PR_PRIVATE
%token TK_PR_PUBLIC
%token TK_PR_PROTECTED
%token TK_PR_END
%token TK_PR_DEFAULT
%token TK_OC_LE
%token TK_OC_GE
%token TK_OC_EQ
%token TK_OC_NE
%token TK_OC_AND
%token TK_OC_OR
%token TK_OC_SL
%token TK_OC_SR
%token TK_OC_FORWARD_PIPE
%token TK_OC_BASH_PIPE
%token TK_LIT_INT
%token TK_LIT_FLOAT
%token TK_LIT_FALSE
%token TK_LIT_TRUE
%token TK_LIT_CHAR
%token TK_LIT_STRING
%token TK_IDENTIFICADOR
%token TOKEN_ERRO

/* precedencia da menor pra maior */

%right '='

%left '+' '-'
%left '*' '/' '%' 


%start programa


/*
TODO: Alterar funcionamento do scanner.l para parar de reconhecer + e - na frente dos TK_INT_LIT
Alterar FLOAT e outras regras que usassem isso
*/

%%

programa: declaracao
        | programa declaracao;

declaracao: var_global
          | funcao_global;

static_opcional: TK_PR_STATIC
               | %empty;

const_opcional: TK_PR_CONST
              | %empty;

tipo: TK_PR_INT
    | TK_PR_FLOAT
    | TK_PR_CHAR
    | TK_PR_BOOL
    | TK_PR_STRING;

var_global: static_opcional tipo ids ';';

ids: ids ',' id_global
   | id_global;

id_global: TK_IDENTIFICADOR
         | TK_IDENTIFICADOR '[' TK_LIT_INT ']';

funcao_global: header_func_global bloco;

header_func_global: static_opcional tipo TK_IDENTIFICADOR '(' params_list_global ')';

params_list_global: global_args_list
                  | %empty;

global_args_list: global_args_list ',' global_func_arg
                | global_func_arg;

global_func_arg: const_opcional tipo TK_IDENTIFICADOR;

bloco: '{' comando_list '}';

comando_list: comando ';' comando_list
        | %empty;

comando: var_local
       | atrib
       | fluxo
       | comando_es
       | func_call
       | shift
       | retorno
       | bloco;

var_local: static_opcional const_opcional tipo variavel;

variavel: variavel ',' TK_IDENTIFICADOR
        | TK_IDENTIFICADOR
        | TK_IDENTIFICADOR TK_OC_LE lit_ou_id;

id_expr: TK_IDENTIFICADOR
       | TK_IDENTIFICADOR '[' expressao ']';

atrib: id_expr '=' expressao;

fluxo: if
     | for
     | while_do;

if: TK_PR_IF '(' expressao ')' bloco else_opcional;
else_opcional: TK_PR_ELSE bloco
             | %empty;

for: TK_PR_FOR '(' atrib ':' expressao ':' atrib ')' bloco;

while_do: TK_PR_WHILE '(' expressao ')' TK_PR_DO bloco;

comando_es: TK_PR_INPUT TK_IDENTIFICADOR
          | TK_PR_OUTPUT lit_ou_id;

func_call: TK_IDENTIFICADOR '(' args_list ')';

args_list: id_or_exp_list
         | %empty;

id_or_exp_list: id_or_exp_list ',' expressao
	      | expressao;

shift: TK_IDENTIFICADOR TK_OC_SR TK_LIT_INT
     | TK_IDENTIFICADOR TK_OC_SL TK_LIT_INT;

retorno: TK_PR_RETURN expressao
       | TK_PR_BREAK
       | TK_PR_CONTINUE;

lit_ou_id: literal | TK_IDENTIFICADOR;

literal: TK_LIT_INT
       | TK_LIT_FLOAT
       | TK_LIT_FALSE
       | TK_LIT_TRUE
       | TK_LIT_CHAR
       | TK_LIT_STRING;

expressao: 
	parenteses_ou_operando operador_binario expressao
	| parenteses_ou_operando;
	
parenteses_ou_operando:
	'(' expressao ')'
	| operandos
	| operador_unario parenteses_ou_operando
    | func_call;

operandos:
	id_expr
	| TK_LIT_INT
	| TK_LIT_FLOAT
	| TK_LIT_TRUE
	| TK_LIT_FALSE;

operador_unario:
	'+'
	| '-'
	| '!'
	| '&'
	| '*'
	| '?'
	| '#';

operador_binario:
	'+'
	| '-'
	| '*'
	| '/'
	| '%'
	| '|'
	| '&'
	| '^'
	| TK_OC_LE
	| TK_OC_GE
	| TK_OC_EQ
	| TK_OC_NE
	| TK_OC_AND
	| TK_OC_OR
	| TK_OC_SL
	| TK_OC_SR
	| '?' expressao ':';


%%

    int yyerror (char const *s){
	printf("Line %d, Column %d:\t%s\n", get_line_number(), get_column_number(), s);

    return -1;
}
