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

    bool tstatic;
    bool tconst;

    Node* node;
    ParamNode* param;

}

%define parse.error verbose

//TERMINAIS (token)

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
%token<valor_lexico.valor.bin_op> TK_OC_LE
%token<valor_lexico.valor.bin_op> TK_OC_GE
%token<valor_lexico.valor.bin_op> TK_OC_EQ
%token<valor_lexico.valor.bin_op> TK_OC_NE
%token<valor_lexico.valor.bin_op> TK_OC_AND
%token<valor_lexico.valor.bin_op> TK_OC_OR
%token<valor_lexico.valor.bin_op> TK_OC_SL
%token<valor_lexico.valor.bin_op> TK_OC_SR
%token<valor_lexico.valor.bin_op> TK_OC_FORWARD_PIPE
%token<valor_lexico.valor.bin_op> TK_OC_BASH_PIPE
%token<valor_lexico.valor.int_lit> TK_LIT_INT
%token<valor_lexico.valor.float_lit> TK_LIT_FLOAT
%token<valor_lexico.valor.bool_lit> TK_LIT_FALSE
%token<valor_lexico.valor.bool_lit> TK_LIT_TRUE
%token<valor_lexico.valor.char_lit> TK_LIT_CHAR
%token<valor_lexico.valor.string_lit> TK_LIT_STRING
%token<valor_lexico.valor.identificador> TK_IDENTIFICADOR
%token TOKEN_ERRO

//Não-Terminais

%type<node> programa
%type<node> declaracao

// %type<tstatic> static_opcional
// %type<tconst> const_opcional

//%type<??> tipo

%type<node> var_global
%type<node> ids
%type<node> id_global
%type<node> funcao_global
//header  não deveria ser nodo, como fazer?
%type<node> header_func_global
%type<node> params_list_global
%type<node> global_args_list
%type<node> global_func_arg
%type<node> bloco
%type<node> comando_list
%type<node> comando
%type<node> var_local
%type<node> variavel
%type<node> id_expr
%type<node> atrib
%type<node> fluxo
%type<node> if
%type<node> else_opcional
%type<node> for
%type<node> while_do
%type<node> comando_es
%type<node> func_call
%type<node> args_list
%type<node> id_or_exp_list
%type<node> shift
%type<node> retorno
%type<node> lit_ou_id
%type<node> literal
%type<node> expressao
%type<node> parenteses_ou_operando
%type<node> operandos
%type<Un_Op_Type> operador_unario
%type<Bin_Op_Type> operador_binario
// %type tipo




/* precedencia da menor pra maior */

%right '='

%left '+' '-'
%left '*' '/' '%' 


%start programa

%initial-action {
    arvore = $$;
};

/* arvore (external) comeca a apontar pra cabeca do parse */

/*
TODO: Alterar funcionamento do scanner.l para parar de reconhecer + e - na frente dos TK_INT_LIT
Alterar FLOAT e outras regras que usassem isso
*/

%%

programa: declaracao
        | programa declaracao;

declaracao: var_global //rever pois pode haver mais de uma declaração em uma única linha
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

var_global: static_opcional tipo ids ';'; { $$ = make_global_var() }

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

id_expr: TK_IDENTIFICADOR { $$ = make_identificador($1, NULL); }
       | TK_IDENTIFICADOR '[' expressao ']' { $$ = make_identificador($1, $3); };

atrib: id_expr '=' expressao { $$ = make_attrib($1, $3);}; 

fluxo: if
     | for
     | while_do;

if: TK_PR_IF '(' expressao ')' bloco else_opcional { $$ = make_if( $3, $5, $6);};
else_opcional: TK_PR_ELSE bloco { $$ = make_block($2);}
             | %empty{ $$ = NULL;};

for: TK_PR_FOR '(' atrib ':' expressao ':' atrib ')' bloco{ $$ = make_for($3,$5,$7,$9);};

while_do: TK_PR_WHILE '(' expressao ')' TK_PR_DO bloco{ $$ = make_while($3,$6);};

comando_es: TK_PR_INPUT TK_IDENTIFICADOR { $$ = make_input($2); }
          | TK_PR_OUTPUT lit_ou_id { $$ = make_output($2);};

func_call: TK_IDENTIFICADOR '(' args_list ')'{ $$ = make_func_call($1, $3);};

args_list: id_or_exp_list
         | %empty { $$ = NULL;};

id_or_exp_list: id_or_exp_list ',' expressao
	      | expressao { $$ = $1;};

shift: TK_IDENTIFICADOR TK_OC_SR TK_LIT_INT
     | TK_IDENTIFICADOR TK_OC_SL TK_LIT_INT;

retorno: TK_PR_RETURN expressao { $$ = make_return($2);}
       | TK_PR_BREAK { $$ = make_break();}
       | TK_PR_CONTINUE { $$ = make_continue();};

lit_ou_id: literal { $$ = $1; }
	| TK_IDENTIFICADOR { $$ = make_identificador($1, NULL); };

literal: TK_LIT_INT { $$ = make_int($1); }
       | TK_LIT_FLOAT { $$ = make_float($1); }
       | TK_LIT_FALSE { $$ = make_bool($1); }
       | TK_LIT_TRUE { $$ = make_bool($1); }
       | TK_LIT_CHAR { $$ = make_char($1); }
       | TK_LIT_STRING { $$ = make_string($1); };

expressao: 
	parenteses_ou_operando operador_binario expressao { $$ = make_bin_op($2, $1, $3);}
	| parenteses_ou_operando { $$ = $1; };
	
parenteses_ou_operando:
	'(' expressao ')' { $$ = $2;}
	| operandos { $$ = $1;}
	| operador_unario parenteses_ou_operando { make_un_op($1,$2);}
    | func_call { $$ = $1;};

operandos:
	id_expr { $$ = $1;}
	| TK_LIT_INT { $$ = make_int($1); }
	| TK_LIT_FLOAT { $$ = make_float($1); }
	| TK_LIT_TRUE { $$ = make_bool($1); }
	| TK_LIT_FALSE { $$ = make_bool($1); };

operador_unario:
	'+' { $$ = Un_Op_Type.PLUS; }
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
