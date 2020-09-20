
%{
#include "parser.tab.h"

static int line_count = 1;
%}

white [ \t]+
digit [0-9]
letter [a-zA-Z]
alphanumeric {letter}|{digit}

char_esp [,;:\(\)\[\]\{\}\+\-|*/<>=!&%#ˆ.$]
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

%%

"int" { return TK_PR_INT; }
"float" { return TK_PR_FLOAT; }
"bool" { return TK_PR_BOOL; }
"char" { return TK_PR_CHAR; }
"string" { return TK_PR_STRING; }
"if" { return TK_PR_IF; }
"then" { return TK_PR_THEN; }
"else" { return TK_PR_ELSE; }
"while" { return TK_PR_WHILE; }
"do" { return TK_PR_DO; }
"input" { return TK_PR_INPUT; }
"output" { return TK_PR_OUTPUT; }
"return" { return TK_PR_RETURN; }
"const" { return TK_PR_CONST; }
"static" { return TK_PR_STATIC; }
"foreach" { return TK_PR_FOREACH; }
"for" { return TK_PR_FOR; }
"switch" { return TK_PR_SWITCH; }
"case" { return TK_PR_CASE; }
"break" { return TK_PR_BREAK; }
"continue" { return TK_PR_CONTINUE; }
"class" { return TK_PR_CLASS; }
"private" { return TK_PR_PRIVATE; }
"public" { return TK_PR_PUBLIC; }
"protected" { return TK_PR_PROTECTED; }
"end" { return TK_PR_END; }
"default" { return TK_PR_DEFAULT; }

"<=" { return TK_OC_LE; }
">=" { return TK_OC_GE; }
"==" { return TK_OC_EQ; }
"!=" { return TK_OC_NE; }
"&&" { return TK_OC_AND; }
"||" { return TK_OC_OR; }
">>" { return TK_OC_SR; }
"<<" { return TK_OC_SL; }

{intlit} { return TK_LIT_INT; }
{floatlit} { return TK_LIT_FLOAT; }
false { return TK_LIT_FALSE; }
true { return TK_LIT_TRUE; }

'({asciialphabet}|{controlchar})' { return TK_LIT_CHAR; }
\"[^\"\n]*\" { return TK_LIT_STRING; }

{id} { return TK_IDENTIFICADOR; }

{char_esp} { return yytext[0]; }
{linebreak} { line_count++; }

{white} 

{inlinecomment}

{block_start} { BEGIN(MULTILINE_COMMENT); }
<MULTILINE_COMMENT>{block_end} { BEGIN(INITIAL); }
<MULTILINE_COMMENT>{linebreak} { line_count++; }
<MULTILINE_COMMENT>. 


. { return TOKEN_ERRO; }


%%

int get_line_number() {
    return line_count;
}

void yyerror (char const *s) {
    fprintf(stderr, "%s in line %d\n\tat: '%s'", s, get_line_number(), yytext);
}
