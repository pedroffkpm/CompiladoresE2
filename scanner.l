
%{
#include "parser.tab.h"

static int line_count = 1;
static int column_count = 0;
void update_line_column();

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

"int" { update_line_column(); return TK_PR_INT; }
"float" { update_line_column(); return TK_PR_FLOAT; }
"bool" { update_line_column(); return TK_PR_BOOL; }
"char" { update_line_column(); return TK_PR_CHAR; }
"string" { update_line_column(); return TK_PR_STRING; }
"if" { update_line_column(); return TK_PR_IF; }
"then" { update_line_column(); return TK_PR_THEN; }
"else" { update_line_column(); return TK_PR_ELSE; }
"while" { update_line_column(); return TK_PR_WHILE; }
"do" { update_line_column(); return TK_PR_DO; }
"input" { update_line_column(); return TK_PR_INPUT; }
"output" { update_line_column(); return TK_PR_OUTPUT; }
"return" { update_line_column(); return TK_PR_RETURN; }
"const" { update_line_column(); return TK_PR_CONST; }
"static" { update_line_column(); return TK_PR_STATIC; }
"foreach" { update_line_column(); return TK_PR_FOREACH; }
"for" { update_line_column(); return TK_PR_FOR; }
"switch" { update_line_column(); return TK_PR_SWITCH; }
"case" { update_line_column(); return TK_PR_CASE; }
"break" { update_line_column(); return TK_PR_BREAK; }
"continue" { update_line_column(); return TK_PR_CONTINUE; }
"class" { update_line_column(); return TK_PR_CLASS; }
"private" { update_line_column(); return TK_PR_PRIVATE; }
"public" { update_line_column(); return TK_PR_PUBLIC; }
"protected" { update_line_column(); return TK_PR_PROTECTED; }
"end" { update_line_column(); return TK_PR_END; }
"default" { update_line_column(); return TK_PR_DEFAULT; }

"<=" { update_line_column(); return TK_OC_LE; }
">=" { update_line_column(); return TK_OC_GE; }
"==" { update_line_column(); return TK_OC_EQ; }
"!=" { update_line_column(); return TK_OC_NE; }
"&&" { update_line_column(); return TK_OC_AND; }
"||" { update_line_column(); return TK_OC_OR; }
">>" { update_line_column(); return TK_OC_SR; }
"<<" { update_line_column(); return TK_OC_SL; }

{intlit} { update_line_column(); return TK_LIT_INT; }
{floatlit} { update_line_column(); return TK_LIT_FLOAT; }
false { update_line_column(); return TK_LIT_FALSE; }
true { update_line_column(); return TK_LIT_TRUE; }

'({asciialphabet}|{controlchar})' { update_line_column(); return TK_LIT_CHAR; }
\"[^\"\n]*\" { update_line_column(); return TK_LIT_STRING; }

{id} { update_line_column(); return TK_IDENTIFICADOR; }

{char_esp} { update_line_column(); return yytext[0]; }
{linebreak} { update_line_column(); }

{white} { update_line_column(); }

{inlinecomment} { update_line_column(); }

{block_start} { update_line_column(); BEGIN(MULTILINE_COMMENT); }
<MULTILINE_COMMENT>{block_end} { update_line_column(); BEGIN(INITIAL); }
<MULTILINE_COMMENT>{linebreak} { update_line_column();  }
<MULTILINE_COMMENT>. { update_line_column(); }


. { update_line_column(); return TOKEN_ERRO; }


%%

int get_line_number() {
    return line_count;
}

int get_column_number() {
    return column_count;
}

void update_line_column() {
    int i;

    for(i=0; yytext[i] != '\0'; i++) {
        if(yytext[i] == '\n') {
            column_count = 0;
            line_count++;
        } else {
            column_count++;
        }
    }
}
