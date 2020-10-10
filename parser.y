%{
#include <stdio.h>
#include "tree.h"
#include "lexVal.h"
int yylex(void);
extern int get_line_number(void); // avisa que função deve ser lincada e está em outro arquivo
int yyerror (char const *s){
	printf("%s, on line %d\n", s, get_line_number());
	return -1;
}

extern void* arvore;

int parsingSucceded = FALSE;
extern Node *danglingNodes;

%}
%define parse.error verbose
%verbose

%union {
	struct lexval* valor_lexico;
	struct node* ast;
}
//Todos os nodos tem valor léxico, portanto o tipo do seu valor semântico será struct lexval representado
//por valor_lexico do %union acima e será adicionado aos tokens
%token <valor_lexico> TK_PR_INT
%token <valor_lexico> TK_PR_FLOAT
%token <valor_lexico> TK_PR_BOOL
%token <valor_lexico> TK_PR_CHAR
%token <valor_lexico> TK_PR_STRING
%token <valor_lexico> TK_PR_IF
%token <valor_lexico> TK_PR_THEN
%token <valor_lexico> TK_PR_ELSE
%token <valor_lexico> TK_PR_WHILE
%token <valor_lexico> TK_PR_DO
%token <valor_lexico> TK_PR_INPUT
%token <valor_lexico> TK_PR_OUTPUT
%token <valor_lexico> TK_PR_RETURN
%token <valor_lexico> TK_PR_CONST
%token <valor_lexico> TK_PR_STATIC
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
%token <valor_lexico> TOKEN_ERRO
//tokens para caracteres especiais, declarados para poder usar seu valor semântico atribuido no scanner
%token <valor_lexico> ',' ';' ':' '(' ')' '[' ']' '{' '}' '+' '-' '|' '?' '*' '/' '<' '>' '=' '!' '&' '%' '#' '^' '.' '$'
%start programa

//Regras, em ordem alfabética, cujo tipo será ast, ou seja, seu valor semântico é representado como uma árvore


%type <ast> args
%type <ast> argsAndCommands
%type <ast> argCall
%type <ast> argsCall
%type <ast> assignment
%type <ast> blocoComandos
%type <ast> case
%type <ast> campo
%type <ast> comando
%type <ast> comandos
%type <ast> comandoSimples
%type <ast> comandosSemVirgula
%type <ast> componente
%type <ast> componentes
%type <ast> continueOutput
%type <ast> depoisDeIdent
%type <ast> do_while
%type <ast> encapsulamento
%type <ast> expression
%type <ast> fechaVarOuFunc
%type <ast> for
%type <ast> foreach
%type <ast> foreachList
%type <ast> forList
%type <ast> funcCall
%type <ast> ifThenElse
%type <ast> infiniteQuestionMarks
%type <ast> input
%type <ast> list
%type <ast> listaCampos
%type <ast> literais
%type <ast> localVarDefinition
%type <ast> negativeOrPositiveIdentifier
%type <ast> negativeOrPositiveLiteral
%type <ast> novoTipo
%type <ast> operands
%type <ast> operators
%type <ast> optElse
%type <ast> output
%type <ast> parameter
%type <ast> parameters
%type <ast> parenthesisOrOperand
%type <ast> pipe
%type <ast> programa
%type <ast> return
%type <ast> shift
%type <ast> shiftOp
%type <ast> static
%type <ast> switch
%type <ast> tamanhoVetor
%type <ast> tipo
%type <ast> tipoConst
%type <ast> tiposPrimitivos
%type <ast> while_do
%destructor { /* Nao podemos desalocar os tokens aqui, precisamos apenas da liberaDanglingScanner();
		Se tentarmos fazer algo aqui, acabamos tentando desalocar alguns tokens mais de uma vez.
		Minha conclusao eh de que o token, apesar de ter ocorrido erro no parsing, eh posto em um nodo
		por alguma das acoes que foram finalizadas antes de se achar o erro. Com isso, acabariamos
		tentando desalocar o mesmo token tanto aqui quanto no destrutor de <ast>.
		OBS.: liberaDanglingParser() desaloca os nodos criados no parser que nao sao adicionados
		      à arvore, assim como os tokens presentes nesses nodos. Ai que mora o problema de
		      tentar liberar o mesmo token mais de uma vez se fizessemos algo no destrutor atual.
	  */} <valor_lexico>
%destructor {
	if(parsingSucceded == FALSE){
		liberaDanglingParser($$);
	}
} <ast>
%%

////////////////////////////////////////////////////////////////////////////////
/// OBSERVAÇÕES SOBRE AS AÇÕES BISON DEFINIDAS A SEGUIR:                     ///
/// 1 - AS AÇÕES FAZEM USO EXAUSTIVO DAS FUNÇÕES DEFINIDAS EM tree.h PARA    ///
///     CONSTRUIR A AST.                                                     ///
/// 2 - A FUNÇÃO Node* criaNodo(struct lexval* token); É USADA PARA CRIAR UM ///
///     NODO DA AST A PARTIR DO VALOR ASSOCIADO A UM TOKEN NO scanner.l      ///
/// 3 - A FUNÇÃO void adicionaFilho(Node *pai, Node *kid); É USADA PARA      ///
///     ADICIONAR UM NODO FILHO AO NODO ATUAL NA AST.                        ///
/// 4 - NO CASO DE PRODUÇÕES VAZIAS SE CRIA UM NODO COM VALOR DE TOKEN       ///
///     NULL.                                                                ///
/// 5 - A ÁRVORE FOI CONSTRUÍDA DE FORMA QUE NÃO EXISTAM NODOS               ///
///     INTERMEDIÁRIOS, COMO EXISTIRIA EM UMA ÁRVORE DE DERIVAÇÃO.           ///
/// 6 - A AST CORRESPONDENTE À PRODUÇÕES CUJO CORPO TENHA MAIS DE UM SÍMBOLO ///
///     É CONSTRUÍDA DE FORMA QUE O PRIMEIRO SÍMBOLO DA ESQUERDA PARA A      ///
///     DIREITA SEJA O PAI DE TODOS OS DEMAIS SÍMBOLOS DA PRODUÇÃO.          ///
////////////////////////////////////////////////////////////////////////////////

programa: 
	componentes					{$$ = $1; arvore = $$; parsingSucceded = TRUE;}
;

componentes: 
	%empty 						{$$ = criaNodo(NULL);}
	| componente componentes 	{ $$ = $1; adicionaFilho($$, $2);}
;
componente:
	  novoTipo							{$$ = $1;}
	| TK_IDENTIFICADOR depoisDeIdent { // Regra introduzida para resolver conflitos
		$$ = criaNodo($1); 
		adicionaFilho($$, $2);
	}
	// Funções
	| tiposPrimitivos TK_IDENTIFICADOR argsAndCommands {
		$$ = $1; adicionaFilho($$, criaNodo($2)); 
		adicionaFilho($$, $3);
	}
	| TK_PR_STATIC tipo TK_IDENTIFICADOR argsAndCommands {
		$$ = criaNodo($1); 
		adicionaFilho($$, $2); 
		adicionaFilho($$, criaNodo($3)); 
		adicionaFilho($$, $4);
	}
;

depoisDeIdent:
	 tamanhoVetor static tipo ';' {
		$$ = $1;
		adicionaFilho($$, $2); 
		adicionaFilho($$, $3); 
		adicionaFilho($$, criaNodo($4));
	}
	| TK_PR_STATIC tipo ';'	{
		$$ = criaNodo($1); 
		adicionaFilho($$, $2); 
		adicionaFilho($$, criaNodo($3));
	}
	| tiposPrimitivos ';' {
		$$ = $1; 
		adicionaFilho($$, criaNodo($2));
	}
	| TK_IDENTIFICADOR fechaVarOuFunc {
		$$ = criaNodo($1); 
		adicionaFilho($$, $2);
	}
;
fechaVarOuFunc:
	  ';'				{$$ = criaNodo($1);}
	| argsAndCommands	{$$ = $1;}
;

//Regras gerais
encapsulamento: 
	%empty 				{$$ = criaNodo(NULL);}
	| TK_PR_PRIVATE 	{$$ = criaNodo($1);}
	| TK_PR_PUBLIC 		{$$ = criaNodo($1);}
	| TK_PR_PROTECTED	{$$ = criaNodo($1);}
;
tiposPrimitivos: 
	TK_PR_INT 			{$$ = criaNodo($1);}
	| TK_PR_FLOAT 		{$$ = criaNodo($1);}
	| TK_PR_BOOL 		{$$ = criaNodo($1);}
	| TK_PR_CHAR 		{$$ = criaNodo($1);}
	| TK_PR_STRING		{$$ = criaNodo($1);}
;
tipo : 
	tiposPrimitivos 	{$$ = $1;}
	| TK_IDENTIFICADOR	{$$ = criaNodo($1);}// TK_IDENTIFICADOR para tipo do usuário
; 
static: 
	TK_PR_STATIC 		{$$ = criaNodo($1);}
	| %empty			{$$ = criaNodo(NULL);} 
;
tipoConst: 
	TK_PR_CONST tipo 	{$$ = criaNodo($1); adicionaFilho($$, $2);} 
	| tipo				{$$ = $1;}
;
literais: 
	TK_LIT_INT 			{$$ = criaNodo($1);}
	| TK_LIT_FLOAT 		{$$ = criaNodo($1);}
	| TK_LIT_FALSE 		{$$ = criaNodo($1);}
	| TK_LIT_TRUE 		{$$ = criaNodo($1);}
	| TK_LIT_CHAR 		{$$ = criaNodo($1);}
	| TK_LIT_STRING		{$$ = criaNodo($1);}
;

//Novos tipos
novoTipo: 
	TK_PR_CLASS TK_IDENTIFICADOR listaCampos ';' {
		$$ = criaNodo($1); 
		adicionaFilho($$, criaNodo($2)); 
		adicionaFilho($$, $3); 
		adicionaFilho($$, criaNodo($4));
	};
listaCampos: 
	'[' list ']'{
		$$ = criaNodo($1); 
		adicionaFilho($$, $2); 
		adicionaFilho($$, criaNodo($3));
	}
;
list: 
	campo 				{$$ = $1;}
	| campo ':' list{
		$$ = $1; 
		adicionaFilho($$, criaNodo($2)); 
		adicionaFilho($$, $3);
	}
;
campo: 
	encapsulamento tiposPrimitivos TK_IDENTIFICADOR	{
		$$ = $1; 
		adicionaFilho($$, $2); 
		adicionaFilho($$, criaNodo($3));
	}
;

//Variáveis globais
tamanhoVetor: 
	'[' TK_LIT_INT ']'	{
		$$ = criaNodo($1); 
		adicionaFilho($$, criaNodo($2)); 
		adicionaFilho($$, criaNodo($3));
	}
;
//Funções
args: 
	%empty 				{$$ = criaNodo(NULL);}
	| parameters		{$$ = $1;}
;		
parameters : 
	parameter ',' parameters {
		$$ = $1; 
		adicionaFilho($$, criaNodo($2)); 
		adicionaFilho($$, $3);
	}
	| parameter					{$$ = $1;}
;
parameter: 
	tipoConst TK_IDENTIFICADOR	{$$ = $1; adicionaFilho($$, criaNodo($2));}
;
argsAndCommands : 
	'(' args ')' blocoComandos {
		$$ = criaNodo($1); 
		adicionaFilho($$, $2); 
		adicionaFilho($$, criaNodo($3)); 
		adicionaFilho($$, $4);
	}
;
//Bloco de comandos
/**
	Observações gerais sobre comandos:
		* Precisei alterar as regras para comandos pois na especificação existem várias excessões de comandos que podem ou não podem ser aplicados em determinado lugar, separei o case dos comandos simples por não ter ';' e separei os que tem vírgula dos que não tem, pois somente os que não tem vírgula podem aparecer nas listas de comando do for
*/
blocoComandos:
	'{' comandos '}' {
		$$ = criaNodo($1); 
		adicionaFilho($$, $2); 
		adicionaFilho($$, criaNodo($3));
	}
;
comando:
	comandoSimples ';'		{$$ = $1; adicionaFilho($$, criaNodo($2));}
	| case					{$$ = $1;}
; //Coloquei a regra do case aqui pois na especificação ele não está atrelado ao switch, mas apenas como marcador de lugar além disso não possui ';' no final e não pode ser usado no for
comandos :
	%empty					{$$ = criaNodo(NULL);}
	| comando comandos		{$$ = $1; adicionaFilho($$, $2);}
;


comandoSimples:
	comandosSemVirgula		{$$ = $1;}
	| output				{$$ = $1;}
	| funcCall				{$$ = $1;}
	| foreach				{$$ = $1;}
	| for					{$$ = $1;}
;


comandosSemVirgula: //comandos que são permitidos dentro das listas do for
	localVarDefinition		{$$ = $1;}
	| assignment			{$$ = $1;}
	| input					{$$ = $1;}
	| shift					{$$ = $1;}
	| TK_PR_BREAK			{$$ = criaNodo($1);}
	| TK_PR_CONTINUE		{$$ = criaNodo($1);}
	| return				{$$ = $1;}
	| ifThenElse			{$$ = $1;}
	| while_do				{$$ = $1;}
	| do_while				{$$ = $1;}
	| switch				{$$ = $1;}
	| pipe					{$$ = $1;}
	| blocoComandos			{$$ = $1;}
;

ifThenElse:
	TK_PR_IF '(' expression ')' TK_PR_THEN blocoComandos optElse {
		$$ = criaNodo($1); 
		adicionaFilho($$, criaNodo($2)); 
		adicionaFilho($$, $3); 
		adicionaFilho($$, criaNodo($4)); 
		adicionaFilho($$, criaNodo($5)); 
		adicionaFilho($$, $6); 
		adicionaFilho($$, $7);
	}
;
optElse:
	%empty							{$$ = criaNodo(NULL);}
	| TK_PR_ELSE blocoComandos		{$$ = criaNodo($1); adicionaFilho($$, $2);}
;
foreach:
	TK_PR_FOREACH '(' TK_IDENTIFICADOR ':' foreachList ')' blocoComandos {
		$$ = criaNodo($1); 
		adicionaFilho($$, criaNodo($2)); 
		adicionaFilho($$, criaNodo($3)); 
		adicionaFilho($$, criaNodo($4)); 
		adicionaFilho($$, $5); 
		adicionaFilho($$, criaNodo($6)); 
		adicionaFilho($$, $7);
	}
;

for:
	TK_PR_FOR '(' forList ':' expression ':' forList ')' blocoComandos	{
		$$ = criaNodo($1); 
		adicionaFilho($$, criaNodo($2)); 
		adicionaFilho($$, $3); 
		adicionaFilho($$, criaNodo($4)); 
		adicionaFilho($$, $5); 
		adicionaFilho($$, criaNodo($6)); 
		adicionaFilho($$, $7); 
		adicionaFilho($$, criaNodo($8)); 
		adicionaFilho($$, $9);
	}
;

while_do:
	TK_PR_WHILE '(' expression ')' TK_PR_DO blocoComandos {
		$$ = criaNodo($1); 
		adicionaFilho($$, criaNodo($2)); 
		adicionaFilho($$, $3); 
		adicionaFilho($$, criaNodo($4)); 
		adicionaFilho($$, criaNodo($5)); 
		adicionaFilho($$, $6);
	}
;
do_while:
	TK_PR_DO blocoComandos TK_PR_WHILE '(' expression ')' {
		$$ = criaNodo($1); 
		adicionaFilho($$, $2); 
		adicionaFilho($$, criaNodo($3)); 
		adicionaFilho($$, criaNodo($4)); 
		adicionaFilho($$, $5); 
		adicionaFilho($$, criaNodo($6));
	}
;


foreachList:
	expression						{$$ = $1;}
	| foreachList ',' expression {
		$$ = $1; 
		adicionaFilho($$, criaNodo($2)); 
		adicionaFilho($$, $3);
	}
;
forList:
	comandosSemVirgula					{$$ = $1;}
	| forList ',' comandosSemVirgula {
		$$ = $1; 
		adicionaFilho($$, criaNodo($2)); 
		adicionaFilho($$, $3);
	}
;

switch:
	TK_PR_SWITCH '(' expression ')' blocoComandos {
		$$ = criaNodo($1); 
		adicionaFilho($$, criaNodo($2)); 
		adicionaFilho($$, $3); 
		adicionaFilho($$, criaNodo($4)); 
		adicionaFilho($$, $5);
	}
;
case:
	TK_PR_CASE TK_LIT_INT ':' {
		$$ = criaNodo($1); 
		adicionaFilho($$, criaNodo($2)); 
		adicionaFilho($$, criaNodo($3));
	}
;


/*Definição de Variáveis*/
localVarDefinition:
	TK_PR_STATIC TK_IDENTIFICADOR TK_IDENTIFICADOR {
		$$ = criaNodo($1); 
		adicionaFilho($$, criaNodo($2)); 
		adicionaFilho($$, criaNodo($3));
	}
	| TK_PR_CONST TK_IDENTIFICADOR TK_IDENTIFICADOR	{
		$$ = criaNodo($1); 
		adicionaFilho($$, criaNodo($2)); 
		adicionaFilho($$, criaNodo($3));
	}
	| TK_PR_STATIC TK_PR_CONST TK_IDENTIFICADOR TK_IDENTIFICADOR {
		$$ = criaNodo($1); 
		adicionaFilho($$, criaNodo($2)); 
		adicionaFilho($$, criaNodo($3)); 
		adicionaFilho($$, criaNodo($4));
	}
	| TK_IDENTIFICADOR TK_IDENTIFICADOR	{
		$$ = criaNodo($1); 
		adicionaFilho($$, criaNodo($2));
	}
	| TK_PR_STATIC tiposPrimitivos TK_IDENTIFICADOR	{
		$$ = criaNodo($1); 
		adicionaFilho($$, $2); 
		adicionaFilho($$, criaNodo($3));
	}
	| TK_PR_CONST tiposPrimitivos TK_IDENTIFICADOR {
		$$ = criaNodo($1); 
		adicionaFilho($$, $2); 
		adicionaFilho($$, criaNodo($3));
	}
	| TK_PR_STATIC TK_PR_CONST tiposPrimitivos TK_IDENTIFICADOR	{
		$$ = criaNodo($1); 
		adicionaFilho($$, criaNodo($2)); 
		adicionaFilho($$, $3); 
		adicionaFilho($$, criaNodo($4));
	}
	| tiposPrimitivos TK_IDENTIFICADOR {
		$$ = $1; 
		adicionaFilho($$, criaNodo($2));
	}

	| TK_PR_STATIC tiposPrimitivos TK_IDENTIFICADOR TK_OC_LE TK_IDENTIFICADOR {
		$$ = criaNodo($1); 
		adicionaFilho($$, $2); 
		adicionaFilho($$, criaNodo($3)); 
		adicionaFilho($$, criaNodo($4)); 
		adicionaFilho($$, criaNodo($5));
	}
	| TK_PR_STATIC tiposPrimitivos TK_IDENTIFICADOR TK_OC_LE negativeOrPositiveIdentifier {
		$$ = criaNodo($1); 
		adicionaFilho($$, $2); 
		adicionaFilho($$, criaNodo($3)); 
		adicionaFilho($$, criaNodo($4)); 
		adicionaFilho($$, $5);
	}
	| TK_PR_CONST tiposPrimitivos TK_IDENTIFICADOR TK_OC_LE TK_IDENTIFICADOR {
		$$ = criaNodo($1); 
		adicionaFilho($$, $2); 
		adicionaFilho($$, criaNodo($3)); 
		adicionaFilho($$, criaNodo($4)); 
		adicionaFilho($$, criaNodo($5));
	}
	| TK_PR_CONST tiposPrimitivos TK_IDENTIFICADOR TK_OC_LE negativeOrPositiveIdentifier {
		$$ = criaNodo($1); 
		adicionaFilho($$, $2); 
		adicionaFilho($$, criaNodo($3)); 
		adicionaFilho($$, criaNodo($4)); 
		adicionaFilho($$, $5);
	}
	| TK_PR_STATIC TK_PR_CONST tiposPrimitivos TK_IDENTIFICADOR TK_OC_LE TK_IDENTIFICADOR {
		$$ = criaNodo($1); 
		adicionaFilho($$, criaNodo($2)); 
		adicionaFilho($$, $3); 
		adicionaFilho($$, criaNodo($4)); 
		adicionaFilho($$, criaNodo($5)); 
		adicionaFilho($$, criaNodo($6));
	}
	| TK_PR_STATIC TK_PR_CONST tiposPrimitivos TK_IDENTIFICADOR TK_OC_LE negativeOrPositiveIdentifier {
		$$ = criaNodo($1); 
		adicionaFilho($$, criaNodo($2)); 
		adicionaFilho($$, $3); 
		adicionaFilho($$, criaNodo($4)); 
		adicionaFilho($$, criaNodo($5)); 
		adicionaFilho($$, $6);
	}
	| tiposPrimitivos TK_IDENTIFICADOR TK_OC_LE TK_IDENTIFICADOR {
		$$ = $1; 
		adicionaFilho($$, criaNodo($2)); 
		adicionaFilho($$, criaNodo($3)); 
		adicionaFilho($$, criaNodo($4));
	}
	| tiposPrimitivos TK_IDENTIFICADOR TK_OC_LE negativeOrPositiveIdentifier {
		$$ = $1; 
		adicionaFilho($$, criaNodo($2)); 
		adicionaFilho($$, criaNodo($3)); 
		adicionaFilho($$, $4);
	}

	| TK_PR_STATIC TK_PR_CONST tiposPrimitivos TK_IDENTIFICADOR TK_OC_LE literais {
		$$ = criaNodo($1); 
		adicionaFilho($$, criaNodo($2)); 
		adicionaFilho($$, $3); 
		adicionaFilho($$, criaNodo($4)); 
		adicionaFilho($$, criaNodo($5)); 
		adicionaFilho($$, $6);
	}
	| TK_PR_STATIC TK_PR_CONST tiposPrimitivos TK_IDENTIFICADOR TK_OC_LE negativeOrPositiveLiteral {
		$$ = criaNodo($1); 
		adicionaFilho($$, criaNodo($2)); 
		adicionaFilho($$, $3); 
		adicionaFilho($$, criaNodo($4)); 
		adicionaFilho($$, criaNodo($5)); 
		adicionaFilho($$, $6);
	}
	| TK_PR_STATIC tiposPrimitivos TK_IDENTIFICADOR TK_OC_LE literais {
		$$ = criaNodo($1); 
		adicionaFilho($$, $2); 
		adicionaFilho($$, criaNodo($3)); 
		adicionaFilho($$, criaNodo($4)); 
		adicionaFilho($$, $5);
	}
	| TK_PR_STATIC tiposPrimitivos TK_IDENTIFICADOR TK_OC_LE negativeOrPositiveLiteral {
		$$ = criaNodo($1); 
		adicionaFilho($$, $2); 
		adicionaFilho($$, criaNodo($3)); 
		adicionaFilho($$, criaNodo($4)); 
		adicionaFilho($$, $5);
	}
	| TK_PR_CONST tiposPrimitivos TK_IDENTIFICADOR TK_OC_LE literais {
		$$ = criaNodo($1); 
		adicionaFilho($$, $2); 
		adicionaFilho($$, criaNodo($3)); 
		adicionaFilho($$, criaNodo($4)); 
		adicionaFilho($$, $5);
	}
	| TK_PR_CONST tiposPrimitivos TK_IDENTIFICADOR TK_OC_LE negativeOrPositiveLiteral {
		$$ = criaNodo($1); 
		adicionaFilho($$, $2); 
		adicionaFilho($$, criaNodo($3)); 
		adicionaFilho($$, criaNodo($4)); 
		adicionaFilho($$, $5);
	}
	| tiposPrimitivos TK_IDENTIFICADOR TK_OC_LE literais {
		$$ = $1; 
		adicionaFilho($$, criaNodo($2)); 
		adicionaFilho($$, criaNodo($3)); 
		adicionaFilho($$, $4);
	}
	| tiposPrimitivos TK_IDENTIFICADOR TK_OC_LE negativeOrPositiveLiteral {
		$$ = $1; 
		adicionaFilho($$, criaNodo($2)); 
		adicionaFilho($$, criaNodo($3)); 
		adicionaFilho($$, $4);
	}
;
negativeOrPositiveIdentifier:
	'-' negativeOrPositiveIdentifier {
		$$ = criaNodo($1); 
		adicionaFilho($$, $2);
	}
	| '-' TK_IDENTIFICADOR	{
		$$ = criaNodo($1); 
		adicionaFilho($$, criaNodo($2));
	}
	| '+' negativeOrPositiveIdentifier {
		$$ = criaNodo($1); 
		adicionaFilho($$, $2);
	}
	| '+' TK_IDENTIFICADOR{
		$$ = criaNodo($1); 
		adicionaFilho($$, criaNodo($2));
	}
;

negativeOrPositiveLiteral:
	'-' negativeOrPositiveLiteral {
		$$ = criaNodo($1); 
		adicionaFilho($$, $2);
	}
	| '-' TK_LIT_INT {
		$$ = criaNodo($1); 
		adicionaFilho($$, criaNodo($2));
	}
	| '-' TK_LIT_FLOAT {
		$$ = criaNodo($1); 
		adicionaFilho($$, criaNodo($2));
	}
	| '+' negativeOrPositiveLiteral {
		$$ = criaNodo($1); 
		adicionaFilho($$, $2);
	}
	| '+' TK_LIT_INT {
		$$ = criaNodo($1); 
		adicionaFilho($$, criaNodo($2));
	}
	| '+' TK_LIT_FLOAT {
		$$ = criaNodo($1); 
		adicionaFilho($$, criaNodo($2));
	}
;


assignment:
	TK_IDENTIFICADOR '=' expression	{
		$$ = criaNodo($1); 
		adicionaFilho($$, criaNodo($2)); 
		adicionaFilho($$, $3);
	}
	| TK_IDENTIFICADOR '[' expression ']' '=' expression {
		$$ = criaNodo($1); 
		adicionaFilho($$, criaNodo($2)); 
		adicionaFilho($$, $3); 
		adicionaFilho($$, criaNodo($4)); 
		adicionaFilho($$, criaNodo($5)); 
		adicionaFilho($$, $6);
	}
	| TK_IDENTIFICADOR '$' TK_IDENTIFICADOR '=' expression {
		$$ = criaNodo($1); 
		adicionaFilho($$, criaNodo($2)); 
		adicionaFilho($$, criaNodo($3)); 
		adicionaFilho($$, criaNodo($4)); 
		adicionaFilho($$, $5);
	}
	| TK_IDENTIFICADOR '[' expression ']' '$' TK_IDENTIFICADOR '=' expression {
		$$ = criaNodo($1); 
		adicionaFilho($$, criaNodo($2)); 
		adicionaFilho($$, $3); 
		adicionaFilho($$, criaNodo($4)); 
		adicionaFilho($$, criaNodo($5)); 
		adicionaFilho($$, criaNodo($6)); 
		adicionaFilho($$, criaNodo($7)); 
		adicionaFilho($$, $8);
	}	
;
input:
	TK_PR_INPUT expression		{$$ = criaNodo($1); adicionaFilho($$, $2);}
;

output:
	TK_PR_OUTPUT expression continueOutput {
		$$ = criaNodo($1); 
		adicionaFilho($$, $2); 
		adicionaFilho($$, $3);
	}
	| TK_PR_OUTPUT expression {
		$$ = criaNodo($1); 
		adicionaFilho($$, $2);
	}
;
continueOutput: 
	',' expression {
		$$ = criaNodo($1); 
		adicionaFilho($$, $2);
	}
	| ',' expression continueOutput	{
		$$ = criaNodo($1); 
		adicionaFilho($$, $2); 
		adicionaFilho($$, $3);
	}
;
funcCall:
	TK_IDENTIFICADOR '(' argsCall ')'{
		$$ = criaNodo($1); 
		adicionaFilho($$, criaNodo($2)); 
		adicionaFilho($$, $3); 
		adicionaFilho($$, criaNodo($4));
	}
	| TK_IDENTIFICADOR '(' ')' {
		$$ = criaNodo($1); 
		adicionaFilho($$, criaNodo($2)); 
		adicionaFilho($$, criaNodo($3));
	}
;
argsCall:
	argCall					{$$ = $1;}
	| argsCall ',' argCall {
		$$ = $1; 
		adicionaFilho($$, criaNodo($2)); 
		adicionaFilho($$, $3);
	}
;
argCall:
	expression			{$$ = $1;}
	| '.'				{$$ = criaNodo($1);};

shiftOp: 
	TK_OC_SL 			{$$ = criaNodo($1);}
	| TK_OC_SR			{$$ = criaNodo($1);};
shift: 
	TK_IDENTIFICADOR shiftOp expression {
		$$ = criaNodo($1); 
		adicionaFilho($$, $2); 
		adicionaFilho($$, $3);
	}
	| TK_IDENTIFICADOR '$' TK_IDENTIFICADOR shiftOp expression {
		$$ = criaNodo($1); 
		adicionaFilho($$, criaNodo($2));
		adicionaFilho($$, criaNodo($3)); 
		adicionaFilho($$, $4); 
		adicionaFilho($$, $5);
	}
	| TK_IDENTIFICADOR '[' expression ']' shiftOp expression {
		$$ = criaNodo($1); 
		adicionaFilho($$, criaNodo($2));
		adicionaFilho($$, $3); 
		adicionaFilho($$, criaNodo($4)); 
		adicionaFilho($$, $5);
		adicionaFilho($$, $6);
	}
	| TK_IDENTIFICADOR '[' expression ']' '$' TK_IDENTIFICADOR shiftOp expression {
		$$ = criaNodo($1); 
		adicionaFilho($$, criaNodo($2));
		adicionaFilho($$, $3); 
		adicionaFilho($$, criaNodo($4)); 
		adicionaFilho($$, criaNodo($5)); 
		adicionaFilho($$, criaNodo($6)); 
		adicionaFilho($$, $7); 
		adicionaFilho($$, $8);
	}
;

return:
	TK_PR_RETURN expression		{$$ = criaNodo($1); adicionaFilho($$, $2);}
;

expression:
	parenthesisOrOperand operators expression {
		$$ = $1; 
		adicionaFilho($$, $2); 
		adicionaFilho($$, $3);
	}
	| infiniteQuestionMarks operators expression {
		$$ = $1; 
		adicionaFilho($$, $2); 
		adicionaFilho($$, $3);
	}
	| parenthesisOrOperand			{$$ = $1;}
	| infiniteQuestionMarks			{$$ = $1;}
;
infiniteQuestionMarks:
	parenthesisOrOperand '?'		{$$ = $1; adicionaFilho($$, criaNodo($2));}
	| infiniteQuestionMarks '?'		{$$ = $1; adicionaFilho($$, criaNodo($2));}
;
pipe:
	funcCall TK_OC_FORWARD_PIPE funcCall {
		$$ = $1; 
		adicionaFilho($$, criaNodo($2)); 
		adicionaFilho($$, $3);
	}
 	|pipe TK_OC_FORWARD_PIPE funcCall {
		$$ = $1; 
		adicionaFilho($$, criaNodo($2)); 
		adicionaFilho($$, $3);
	}
	|funcCall TK_OC_BASH_PIPE funcCall {
		$$ = $1; 
		adicionaFilho($$, criaNodo($2)); 
		adicionaFilho($$, $3);
	}
	|pipe TK_OC_BASH_PIPE funcCall {
		$$ = $1; 
		adicionaFilho($$, criaNodo($2)); 
		adicionaFilho($$, $3);
	}
;

parenthesisOrOperand:
	'(' expression ')' {
		$$ = criaNodo($1); 
		adicionaFilho($$, $2); 
		adicionaFilho($$, criaNodo($3));
	}
	| operands					{$$ = $1;}
	| '-' parenthesisOrOperand	{$$ = criaNodo($1); adicionaFilho($$, $2);}
	| '+' parenthesisOrOperand	{$$ = criaNodo($1); adicionaFilho($$, $2);}
	| '!' parenthesisOrOperand	{$$ = criaNodo($1); adicionaFilho($$, $2);}
	| '*' parenthesisOrOperand	{$$ = criaNodo($1); adicionaFilho($$, $2);}
	| '&' parenthesisOrOperand	{$$ = criaNodo($1); adicionaFilho($$, $2);}
	| '#' parenthesisOrOperand	{$$ = criaNodo($1); adicionaFilho($$, $2);}
;
operands:
	TK_IDENTIFICADOR '[' expression ']' {
		$$ = criaNodo($1); 
		adicionaFilho($$, criaNodo($2)); 
		adicionaFilho($$, $3); 
		adicionaFilho($$, criaNodo($4));
	}
	| TK_IDENTIFICADOR							{$$ = criaNodo($1);}
	| TK_IDENTIFICADOR '$' TK_IDENTIFICADOR		{
		$$ = criaNodo($1); 
		adicionaFilho($$, criaNodo($2)); 
		adicionaFilho($$, criaNodo($3));
	}
	| TK_IDENTIFICADOR '[' expression ']' '$' TK_IDENTIFICADOR	{
		$$ = criaNodo($1); 
		adicionaFilho($$, criaNodo($2)); 
		adicionaFilho($$, $3); 
		adicionaFilho($$, criaNodo($4)); 
		adicionaFilho($$, criaNodo($5)); 
		adicionaFilho($$, criaNodo($6));
	}
	| TK_LIT_INT		{$$ = criaNodo($1);}
	| TK_LIT_FLOAT		{$$ = criaNodo($1);}
	| TK_LIT_TRUE		{$$ = criaNodo($1);}
	| TK_LIT_FALSE		{$$ = criaNodo($1);}
	| TK_LIT_CHAR		{$$ = criaNodo($1);}		
	| TK_LIT_STRING		{$$ = criaNodo($1);}
	| funcCall			{$$ = $1;}
	| pipe				{$$ = $1;}
;	
operators:
	'+'				{$$ = criaNodo($1);}
	| '-'			{$$ = criaNodo($1);}
	| '*'			{$$ = criaNodo($1);}
	| '/'			{$$ = criaNodo($1);}
	| '%'			{$$ = criaNodo($1);}
	| '^'			{$$ = criaNodo($1);}
	| TK_OC_AND		{$$ = criaNodo($1);}
	| TK_OC_OR		{$$ = criaNodo($1);}
	| '|'			{$$ = criaNodo($1);}
	| '&'			{$$ = criaNodo($1);}
	| TK_OC_EQ		{$$ = criaNodo($1);}
	| TK_OC_NE		{$$ = criaNodo($1);}
	| TK_OC_GE		{$$ = criaNodo($1);}
	| TK_OC_LE		{$$ = criaNodo($1);}
	| '<'			{$$ = criaNodo($1);}
	| '>'			{$$ = criaNodo($1);}
;

%%


