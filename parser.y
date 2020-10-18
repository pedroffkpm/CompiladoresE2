%{
#include <stdio.h>
#include <stdlib.h>
#include "lexVal.h"
#include "lex.yy.h"	
#include "ast.h"
    
#define YYERROR_VERBOSE 1

extern void * arvore;

extern int get_line_number(void);

// extern int get_column_number(void);

int yylex(void);

int yyerror (char const *s);

int parsingSucceded = FALSE;

extern Node *danglingNodes;


%}

%union {
	struct lexval* valor_lexico;
	struct node* ast;
	Type type;
}

%define parse.error verbose

//TERMINAIS (token)

%token <type> TK_PR_INT
%token <type> TK_PR_FLOAT
%token <type> TK_PR_BOOL
%token <type> TK_PR_CHAR
%token <type> TK_PR_STRING
%token <valor_lexico> TK_PR_IF
%token <valor_lexico> TK_PR_THEN
%token <valor_lexico> TK_PR_ELSE
%token <valor_lexico> TK_PR_WHILE
%token TK_PR_DO
%token <valor_lexico> TK_PR_INPUT
%token <valor_lexico> TK_PR_OUTPUT
%token <valor_lexico> TK_PR_RETURN
%token TK_PR_CONST
%token TK_PR_STATIC
%token <valor_lexico> TK_PR_FOREACH
%token <valor_lexico> TK_PR_FOR
%token <valor_lexico> TK_PR_SWITCH
%token <valor_lexico> TK_PR_CASE
%token <valor_lexico> TK_PR_BREAK
%token <valor_lexico> TK_PR_CONTINUE
%token <valor_lexico> TK_PR_CLASS
%token <valor_lexico> TK_PR_PRIVATE
%token <valor_lexico> TK_PR_PUBLIC
%token <valor_lexico> TK_PR_PROTECTED
%token <valor_lexico> TK_PR_END
%token <valor_lexico> TK_PR_DEFAULT
%token <valor_lexico> TK_OC_LE
%token <valor_lexico> TK_OC_GE
%token <valor_lexico> TK_OC_EQ
%token <valor_lexico> TK_OC_NE
%token <valor_lexico> TK_OC_AND
%token <valor_lexico> TK_OC_OR
%token <valor_lexico> TK_OC_SL
%token <valor_lexico> TK_OC_SR
%token <valor_lexico> TK_OC_FORWARD_PIPE
%token <valor_lexico> TK_OC_BASH_PIPE
%token <valor_lexico> TK_LIT_INT
%token <valor_lexico> TK_LIT_FLOAT
%token <valor_lexico> TK_LIT_FALSE
%token <valor_lexico> TK_LIT_TRUE
%token <valor_lexico> TK_LIT_CHAR
%token <valor_lexico> TK_LIT_STRING
%token <valor_lexico> TK_IDENTIFICADOR
%token <valor_lexico>  '[' '+' '-' '|' '?' '*' '/' '<' '>' '=' '!' '&' '%' '#' '^' '.' '$'
%token ',' ';' ':' '(' ')' ']' '{' '}'
%token TOKEN_ERRO

//Não-Terminais

%type <ast> programa
%type <ast> componentes
%type <ast> declaracao

//%type <ast> static_opcional
//%type <ast> const_opcional

//%type<??> tipo

%type <ast> var_global
%type <ast> ids
%type <ast> id_global
%type <ast> funcao_global
//header  não deveria ser nodo, como fazer?
// %type<node> header_func_global
%type <ast> params_list_global
%type <ast> global_args_list
%type <ast> global_func_arg
%type <ast> bloco
%type <ast> comando_list
%type <ast> comando
%type <ast> var_local
%type <ast> init_opcional
%type <ast> variavel
%type <ast> id_expr
%type <ast> atrib
%type <ast> fluxo
%type <ast> if
%type <ast> else_opcional
%type <ast> for
%type <ast> while_do
%type <ast> comando_es
%type <ast> func_call
%type <ast> args_list
%type <ast> id_or_exp_list
%type <ast> shift
%type <ast> retorno
%type <ast> lit_ou_id
%type <ast> literal
%type <ast> expressao
%type <ast> parenteses_ou_operando
%type <ast> operandos
%type <ast> operador_unario
%type <ast> operador_binario
%type <ast> operador_ternario
%type <type> tipo

%destructor {} <valor_lexico>
%destructor {
	if(parsingSucceded == FALSE){
		freeDanglingParser($$);
	}
} <ast>




/* precedencia da menor pra maior */

%right '='

%left '+' '-'
%left '*' '/' '%' 


%start programa


/* arvore (external) comeca a apontar pra cabeca do parse */

//destructors vem aqui

%%
programa:
		componentes { $$ = $1; arvore = $$; parsingSucceded = TRUE; };

componentes: 
	declaracao { $$ = $1; }
        | declaracao componentes { $$ = $1; addChild($$, $2); };

declaracao: 
	var_global { $$ = $1; }
        | funcao_global { $$ = $1; } ;

static_opcional: 
	TK_PR_STATIC {  }
        | %empty {  }; 

const_opcional: 
	TK_PR_CONST { }
        | %empty{ };

tipo: TK_PR_INT { $$ = INT_TYPE; }
    | TK_PR_FLOAT { $$ = FLOAT_TYPE;}
    | TK_PR_CHAR { $$ = CHAR_TYPE;}
    | TK_PR_BOOL { $$ = BOOL_TYPE;}
    | TK_PR_STRING { $$ = STRING_TYPE;};

var_global: static_opcional tipo ids ';' { $$ = $3; 
								addTypeRec($$, $2);}; //reservada

ids: id_global ',' ids {$$ = $1; 
								addChild($$, $3); }
   | id_global { $$ = $1; } ;

id_global: TK_IDENTIFICADOR { $$ = createNode($1, NONE);
								addNature($$, VAR_DEC); }
         | TK_IDENTIFICADOR '[' TK_LIT_INT ']' { $$ = createNode($2, VEC_INDEX);
								addNature($$, VECTOR);
								Node *node1 = createNode($1, NONE);
								addNature(node1, VEC_ID);
								addChild($$, node1);
								Node *node2 = createNode($3, NONE);
								addType(node2, INT_TYPE);
								addNature(node2, VEC_IND); 
								addChild($$, node2); } ;

funcao_global: static_opcional tipo TK_IDENTIFICADOR '(' params_list_global ')' bloco { $$ = createNode($3, NONE); //reservada
								addType($$, $2);
								addNature($$, FUNCTION);
								addChild($$, $5);
								addChild($$, $7); } ;

params_list_global: global_args_list { $$ = $1; }
                  | %empty { $$ = createNode(NULL, NONE); } ;

global_args_list: global_func_arg ',' global_args_list { $$ = $1; 
								addChild($$, $3); }
                | global_func_arg { $$ = $1; } ;

global_func_arg: const_opcional tipo TK_IDENTIFICADOR { $$ = createNode($3, NONE);
								addType($$, $2);
								addNature($$, FUNC_ARG); } ; //reservada

bloco: '{' comando_list '}' { $$ = $2; }; 
	//|comando { $$ = $1; };

comando_list: comando ';' comando_list { $$ = $1; 
								addChild($$, $3);}
        | %empty { $$ = createNode(NULL, NONE); } ;

comando: var_local { $$ = $1; }
       | atrib { $$ = $1; }
       | fluxo { $$ = $1; }
       | comando_es { $$ = $1; }
       | func_call { $$ = $1; }
       | shift { $$ = $1; }
       | retorno { $$ = $1; }
       | bloco { $$ = $1; };

var_local: static_opcional const_opcional tipo variavel { $$ = $4;
								addType($$, $3); } ; //reservada

variavel: TK_IDENTIFICADOR init_opcional ',' variavel { $$ = createNode($1, NONE); //observar(talvez inverter)
								addNature($$, VAR_DEC);
								addChild($$, $2);
								addChild($$, $4); }
        | TK_IDENTIFICADOR init_opcional { $$ = createNode($1, NONE);
								addNature($$, VAR_DEC);
								addChild($$, $2); } ;

init_opcional: TK_OC_LE lit_ou_id { $$ = createNode($1, NONE);
								addChild($$, $2); }
             | %empty { $$ = createNode(NULL, NONE); } ;

id_expr: TK_IDENTIFICADOR { $$ = createNode($1, NONE);
								addNature($$, VAR); }
       | TK_IDENTIFICADOR '[' expressao ']' { $$ = createNode($2, VEC_INDEX);
								addNature($$, VECTOR);
								Node *node = createNode($1, NONE);
								addNature(node, VEC_IND);
								addType(node, $3->token->varType);
								addChild($$, node);
								addNature($3, VEC_ID); 
								addChild($$, $3); } ;

atrib: id_expr '=' expressao { $$ = createNode($2, NONE);
								addChild($$, $1);
								addChild($$, $3); } ;

fluxo: if {$$ = $1;}
     | for {$$ = $1;}
     | while_do {$$ = $1;};

if: TK_PR_IF '(' expressao ')' bloco else_opcional { $$ = createNode($1, NONE);
								addChild($$, $3);
								addChild($$, $5);
								addChild($$, $6);};
else_opcional: TK_PR_ELSE bloco { $$ = createNode($1, NONE);
								addChild($$, $2);}
             | %empty{ $$ = createNode(NULL, NONE);};

for: TK_PR_FOR '(' atrib ':' expressao ':' atrib ')' bloco { $$ = createNode($1, NONE);
								addChild($$, $3);
								addChild($$, $5);
								addChild($$, $7);
								addChild($$, $9);};

while_do: TK_PR_WHILE '(' expressao ')' TK_PR_DO bloco { $$ = createNode($1, NONE);
								addChild($$, $3);
								addChild($$, $6);};

comando_es: TK_PR_INPUT TK_IDENTIFICADOR { $$ = createNode($1, NONE);
								addNature($$, INPUT);
								Node *node = createNode($2, NONE);
								addNature(node, VAR);
								addChild($$, node); }
          | TK_PR_OUTPUT lit_ou_id { $$ = createNode($1, NONE);
								addNature($$, OUTPUT);
								addChild($$, $2);};

func_call: TK_IDENTIFICADOR '(' args_list ')' { $$ = createNode($1, F_CALL);
								addNature($$, FUNCTION);
								addChild($$, $3);};

args_list: id_or_exp_list { $$ = $1;}
         | %empty { $$ = createNode(NULL, NONE);} ;

id_or_exp_list: expressao ',' id_or_exp_list { $$ = $1;
								addChild($$, $3); }
	          | expressao { $$ = $1;} ;

shift: TK_IDENTIFICADOR TK_OC_SR TK_LIT_INT { $$ = createNode($2, NONE);
								Node *node1 = createNode($1, NONE);
								addNature(node1, VAR);
								addType(node1, INT_TYPE);
								addChild($$, node1);
								Node *node2 = createNode($3, NONE);
								addNature(node2, LIT_INT);
								addType(node2, INT_TYPE);
								addChild($$, node2); }
     | TK_IDENTIFICADOR TK_OC_SL TK_LIT_INT { $$ = createNode($2, NONE);
								Node *node1 = createNode($1, NONE);
								addNature(node1, VAR);
								addType(node1, INT_TYPE);
								addChild($$, node1);
								Node *node2 = createNode($3, NONE);
								addNature(node2, LIT_INT);
								addType(node2, INT_TYPE);
								addChild($$, node2); }

retorno: TK_PR_RETURN expressao { $$ = createNode($1, NONE);
								addNature($$, RETURN);
								addType($$, $2->token->varType); 
								addChild($$, $2);}
	|TK_PR_RETURN { $$ = createNode($1, NONE);
								addNature($$, RETURN); }
       	| TK_PR_BREAK { $$ = createNode($1, NONE); }
       	| TK_PR_CONTINUE { $$ = createNode($1, NONE); } ;

lit_ou_id: literal { $$ = $1; }
	| TK_IDENTIFICADOR { $$ = createNode($1, NONE);
								addNature($$, VAR); } ;

literal: TK_LIT_INT { $$ = createNode($1, NONE);
								addNature($$, LIT_INT);
								addType($$, INT_TYPE); }
       	| TK_LIT_FLOAT { $$ = createNode($1, NONE);
								addNature($$, LIT_FLOAT);
								addType($$, FLOAT_TYPE); }
       	| TK_LIT_FALSE { $$ = createNode($1, NONE);
								addNature($$, LIT_BOOL);
								addType($$, BOOL_TYPE); }
       	| TK_LIT_TRUE { $$ = createNode($1, NONE);
								addNature($$, LIT_BOOL);
								addType($$, BOOL_TYPE); }
       	| TK_LIT_CHAR { $$ = createNode($1, NONE);
								addNature($$, LIT_CHAR);
								addType($$, CHAR_TYPE); }
       	| TK_LIT_STRING { $$ = createNode($1, NONE);
								addNature($$, LIT_STRING);
								addType($$, STRING_TYPE); } ;

expressao: 
	parenteses_ou_operando operador_binario expressao { $$ = $2;
								addNature($$, BIN_OP);
								addType($$, $1->token->varType);
								addChild($$, $1);
								addChild($$, $3); }
	|parenteses_ou_operando operador_ternario expressao { $$ = $2;
								addNature($$, TER_OP);
								addType($$, $3->token->varType);
								addChild($$, $1);
								addChild($$, $3); }
	| parenteses_ou_operando { $$ = $1; };

parenteses_ou_operando:
	'(' expressao ')' { $$ = $2;}
	| operandos { $$ = $1;}
	| operador_unario parenteses_ou_operando { $$ = $1, addChild($$, $2);
								addNature($$, UN_OP);
								addType($$, $2->token->varType); }
   	| func_call { $$ = $1; } ;

operandos:
	id_expr { $$ = $1;}
	| TK_LIT_INT { $$ = createNode($1, NONE);
								addNature($$, LIT_INT);
								addType($$, INT_TYPE); }
       	| TK_LIT_FLOAT { $$ = createNode($1, NONE);
								addNature($$, LIT_FLOAT);
								addType($$, FLOAT_TYPE); }
       	| TK_LIT_FALSE { $$ = createNode($1, NONE);
								addNature($$, LIT_BOOL);
								addType($$, BOOL_TYPE); }
       	| TK_LIT_TRUE { $$ = createNode($1, NONE);
								addNature($$, LIT_BOOL);
								addType($$, BOOL_TYPE); };

operador_unario:
	'+' { $$ = createNode($1, NONE); }
	| '-' { $$ = createNode($1, NONE); } 
	| '!' { $$ = createNode($1, NONE); }
	| '&' { $$ = createNode($1, NONE); } 
	| '*' { $$ = createNode($1, NONE); }
	| '?' { $$ = createNode($1, NONE); } 
	| '#' { $$ = createNode($1, NONE); } ;

operador_binario:
	  '+' { $$ = createNode($1, NONE); }
	| '-' { $$ = createNode($1, NONE); } 
	| '*' { $$ = createNode($1, NONE); } 
	| '/' { $$ = createNode($1, NONE); } 
	| '%' { $$ = createNode($1, NONE); } 
	| '|' { $$ = createNode($1, NONE); }
	| '&' { $$ = createNode($1, NONE); } 
	| '^' { $$ = createNode($1, NONE); } 
	| TK_OC_LE { $$ = createNode($1, NONE); }
	| TK_OC_GE { $$ = createNode($1, NONE); } 
	| TK_OC_EQ { $$ = createNode($1, NONE); }
	| TK_OC_NE { $$ = createNode($1, NONE); } 
	| TK_OC_AND { $$ = createNode($1, NONE); } 
	| TK_OC_OR { $$ = createNode($1, NONE); };

operador_ternario:
	'?' expressao ':' { $$ = createNode($1, TERN_OP);
			addChild($$, $2);};

%%

    int yyerror (char const *s){
	printf("Line %d:\t%s\n", get_line_number(), s);

    return -1;
}
