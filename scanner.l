
%{

#include "ast.h" //puxa a bib <stdbool.h> -> permite usar o tipo bool e false/true
#include "parser.tab.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


int createdTokens = 0;
bool dangInit = false;

Node *danglingNodes;

void initDanglingNodes();
void configLexValueNonLiteral(int tokenType);
int get_line_number();


%}

white [ \t]+
digit [0-9]
letter [a-zA-Z]
alphanumeric {letter}|{digit}

char_esp [,;:\(\)\[\]\{\}\+\-|*/<>=!&%#ˆ.$?^]
id {letter}({alphanumeric}|_)*

controlchar "\\0"|"\\t"|"\\n"|"\\v"|"\\f"|"\\r"
asciialphabet {char_esp}|{digit}|{letter}

linebreak \n

inlinecomment \/\/.*

block_start \/\*
block_end \*\/

intlit {digit}+
floatlit {intlit}"."{digit}+((E|e){intlit})?

/* exclusive start conditions */
%x MULTILINE_COMMENT

%option yylineno

%%

"int" { configLexValueNonLiteral(KEYWORD); return TK_PR_INT; }
"float" { configLexValueNonLiteral(KEYWORD); return TK_PR_FLOAT; }
"bool" { configLexValueNonLiteral(KEYWORD); return TK_PR_BOOL; }
"char" { configLexValueNonLiteral(KEYWORD); return TK_PR_CHAR; }
"string" { configLexValueNonLiteral(KEYWORD); return TK_PR_STRING; }
"if" { configLexValueNonLiteral(KEYWORD); return TK_PR_IF; }
"then" { configLexValueNonLiteral(KEYWORD); return TK_PR_THEN; }
"else" { configLexValueNonLiteral(KEYWORD); return TK_PR_ELSE; }
"while" { configLexValueNonLiteral(KEYWORD); return TK_PR_WHILE; }
"do" { configLexValueNonLiteral(KEYWORD); return TK_PR_DO; }
"input" { configLexValueNonLiteral(KEYWORD); return TK_PR_INPUT; }
"output" { configLexValueNonLiteral(KEYWORD); return TK_PR_OUTPUT; }
"return" { configLexValueNonLiteral(KEYWORD); return TK_PR_RETURN; }
"const" { configLexValueNonLiteral(KEYWORD); return TK_PR_CONST; }
"static" { configLexValueNonLiteral(KEYWORD); return TK_PR_STATIC; }
"foreach" { configLexValueNonLiteral(KEYWORD); return TK_PR_FOREACH; }
"for" { configLexValueNonLiteral(KEYWORD); return TK_PR_FOR; }
"switch" { configLexValueNonLiteral(KEYWORD); return TK_PR_SWITCH; }
"case" { configLexValueNonLiteral(KEYWORD); return TK_PR_CASE; }
"break" { configLexValueNonLiteral(KEYWORD); return TK_PR_BREAK; }
"continue" { configLexValueNonLiteral(KEYWORD); return TK_PR_CONTINUE; }
"class" { configLexValueNonLiteral(KEYWORD); return TK_PR_CLASS; }
"private" { configLexValueNonLiteral(KEYWORD); return TK_PR_PRIVATE; }
"public" { configLexValueNonLiteral(KEYWORD); return TK_PR_PUBLIC; }
"protected" { configLexValueNonLiteral(KEYWORD); return TK_PR_PROTECTED; }
"end" { configLexValueNonLiteral(KEYWORD); return TK_PR_END; }
"default" { configLexValueNonLiteral(KEYWORD); return TK_PR_DEFAULT; }

"<=" { configLexValueNonLiteral(COMP_OPER); return TK_OC_LE; }

">=" { configLexValueNonLiteral(COMP_OPER); return TK_OC_GE; }

"==" { configLexValueNonLiteral(COMP_OPER); return TK_OC_EQ; }

"!=" { configLexValueNonLiteral(COMP_OPER); return TK_OC_NE; }

"&&" { configLexValueNonLiteral(COMP_OPER); return TK_OC_AND; }

"||" { configLexValueNonLiteral(COMP_OPER); return TK_OC_OR; }

">>" { configLexValueNonLiteral(COMP_OPER); return TK_OC_SR; }

"<<" { configLexValueNonLiteral(COMP_OPER); return TK_OC_SL; }

{intlit} { 
	initDanglingNodes();
	createdTokens++;
	yylval.valor_lexico = (struct lexval*) malloc(sizeof(struct lexval));
	yylval.valor_lexico->lineNumber = get_line_number();
	yylval.valor_lexico->tokenType = LITERAL;
	yylval.valor_lexico->literalType = INT;
	yylval.valor_lexico->value.i = atoi(yytext);
	yylval.valor_lexico->tokenInAst = FALSE;
	addChild(danglingNodes, createDanglingNode(yylval.valor_lexico));
	return TK_LIT_INT;  }
{floatlit} { 
	initDanglingNodes();
	createdTokens++;
	yylval.valor_lexico = (struct lexval*) malloc(sizeof(struct lexval));
	yylval.valor_lexico->lineNumber = get_line_number();
	yylval.valor_lexico->tokenType = LITERAL;
	yylval.valor_lexico->literalType = FLOAT;
	yylval.valor_lexico->value.f = atof(yytext);
	yylval.valor_lexico->tokenInAst = FALSE;
	addChild(danglingNodes, createDanglingNode(yylval.valor_lexico));
	return TK_LIT_INT; }
false { 
	initDanglingNodes();
	createdTokens++;
	yylval.valor_lexico = (struct lexval*) malloc(sizeof(struct lexval));
	yylval.valor_lexico->lineNumber = get_line_number();
	yylval.valor_lexico->tokenType = LITERAL;
	yylval.valor_lexico->literalType = BOOL;
	yylval.valor_lexico->value.b = FALSE;
	yylval.valor_lexico->tokenInAst = FALSE;
	addChild(danglingNodes, createDanglingNode(yylval.valor_lexico));
	return TK_LIT_TRUE; }
true { 
	initDanglingNodes();
	createdTokens++;
	yylval.valor_lexico = (struct lexval*) malloc(sizeof(struct lexval));
	yylval.valor_lexico->lineNumber = get_line_number();
	yylval.valor_lexico->tokenType = LITERAL;
	yylval.valor_lexico->literalType = BOOL;
	yylval.valor_lexico->value.b = TRUE;
	yylval.valor_lexico->tokenInAst = FALSE;
	addChild(danglingNodes, createDanglingNode(yylval.valor_lexico));
	return TK_LIT_TRUE;  }

'({asciialphabet}|{controlchar})' { 
	initDanglingNodes();
	createdTokens++;
	yylval.valor_lexico = (struct lexval*) malloc(sizeof(struct lexval));
	yylval.valor_lexico->lineNumber = get_line_number();
	yylval.valor_lexico->tokenType = LITERAL;
	yylval.valor_lexico->literalType = CHAR;
	yylval.valor_lexico->value.str = strdup(yytext);
	yylval.valor_lexico->tokenInAst = FALSE;
	addChild(danglingNodes, createDanglingNode(yylval.valor_lexico));
	return TK_LIT_CHAR; }

\"[^\"\n]*\" { 
	initDanglingNodes();
	createdTokens++;
	yylval.valor_lexico = (struct lexval*) malloc(sizeof(struct lexval));
	yylval.valor_lexico->lineNumber = get_line_number();
	yylval.valor_lexico->tokenType = LITERAL;
	yylval.valor_lexico->literalType = STRING;
	yylval.valor_lexico->value.str = strdup(yytext);
	yylval.valor_lexico->tokenInAst = FALSE;
	addChild(danglingNodes, createDanglingNode(yylval.valor_lexico));
	return TK_LIT_STRING; }

{id} { 
	configLexValueNonLiteral(IDS);
	return TK_IDENTIFICADOR; }

{char_esp} { 
	initDanglingNodes();
	createdTokens++;
	yylval.valor_lexico = (struct lexval*) malloc(sizeof(struct lexval));
	yylval.valor_lexico->lineNumber = get_line_number();
	yylval.valor_lexico->tokenType = SPEC_CHAR;
	yylval.valor_lexico->literalType = NONE;
	yylval.valor_lexico->value.c = yytext[0];
	yylval.valor_lexico->tokenInAst = FALSE;
	addChild(danglingNodes, createDanglingNode(yylval.valor_lexico));
    return *yytext; }
{linebreak} { }

{white} { }

{inlinecomment} { }

{block_start} { BEGIN(MULTILINE_COMMENT); }
<MULTILINE_COMMENT>{block_end} {  BEGIN(INITIAL); }
<MULTILINE_COMMENT>{linebreak} { }
<MULTILINE_COMMENT>. { }


. {return TOKEN_ERRO; }


%%

void initDanglingNodes() {
	if(dangInit == FALSE) {
		danglingNodes = createDanglingNode(NULL);
		dangInit = TRUE;
	}
}

void configLexValueNonLiteral(int tokenType) {
	initDanglingNodes();
	createdTokens++;
	yylval.valor_lexico = (struct lexval*) malloc(sizeof(struct lexval));
	yylval.valor_lexico->lineNumber = get_line_number(); 
	yylval.valor_lexico->tokenType = tokenType;
	yylval.valor_lexico->literalType = NONE;
	yylval.valor_lexico->tokenInAst = FALSE;
	if(tokenType != KEYWORD) {
		yylval.valor_lexico->value.str = strdup(yytext); //usa str pois o valor retornado por strdup também é uma cadeia de caracteres
	}
	addChild(danglingNodes, createDanglingNode(yylval.valor_lexico));
}

int get_line_number() {
	return yylineno;
}

