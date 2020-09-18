%{
    #include <stdio.h>
    #include <stdlib.h>
    #include "lex.yy.h"

    extern int get_line_number(void);
    int yylex(void);
    void yyerror (char const *s);
%}

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
        | programa declaracao ;

declaracao: global
          | funcao;

tipo: TK_PR_INT
    | TK_PR_FLOAT
    | TK_PR_CHAR
    | TK_PR_BOOL
    | TK_PR_STRING ;

ids: ids ',' id
   | id ;

id: TK_IDENTIFICADOR
  | TK_IDENTIFICADOR '[' TK_LIT_INT ']' ;

global: TK_PR_STATIC tipo ids ';'
      | tipo ids ';';


funcao: header bloco;

header: TK_PR_STATIC tipo TK_IDENTIFICADOR '(' params ')'
      | tipo TK_IDENTIFICADOR '(' params ')'
      | TK_PR_STATIC tipo TK_IDENTIFICADOR '(' ')'
      | tipo TK_IDENTIFICADOR '(' ')' ;

params: params ',' param
      | param ;

param: TK_PR_CONST tipo TK_IDENTIFICADOR
     | tipo TK_IDENTIFICADOR ;

bloco: '{' comandos '}';

comandos: comando ';' comandos
        | %empty ;

comando: %empty;

%%

// void yyerror (char const *s) {
//     fprintf(stderr, "%s at line %d\n", s, get_line_number());
// }