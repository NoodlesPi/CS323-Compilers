%{
    #include "tree.h"
    #include "syntax.tab.h"

    #define _NEW_LEAF_NODE(name) { \
        printf("%s\n", yytext);\
        yylval.node = newLeafNode(#name, yylineno); \
        return name; \
    }
    #define _ERROR_TYPE_A(text) { \
        fprintf(fout, "Error type A at Line %d: unknown lexeme %s\n", yylineno, text); \
    }

        extern FILE* fout;
%}
%option yylineno

number [-+]?[1-9][0-9]*|0
type int|float|char

id [a-zA-Z_][0-9a-zA-Z_]{0,31}
wrong_id [0-9a-zA-Z_]+

int {number}|0(x|X)([1-9a-fA-F][0-9a-fA-F]{0,7}|0)
wrong_int [-+]?[0-9]+|0(x|X)[0-9a-zA-Z]+

float ({number}?\.[0-9]+)|({number}\.[0-9]*)

char '[ -~]'|'(\\x[0-9a-fA-F]{2})'
wrong_char '\\x[0-9a-zA-Z]*'

empty [ \n\r\t]

single_line_comment \/\/.*
multi_line_comment \/\*([^*]|[\r\n]|(\*+([^*\/]|[\r\n])))*\*+\/
%%

"struct" _NEW_LEAF_NODE(STRUCT)
"if" _NEW_LEAF_NODE(IF)
"else" _NEW_LEAF_NODE(ELSE)
"while" _NEW_LEAF_NODE(WHILE)
"return" _NEW_LEAF_NODE(RETURN)
"." _NEW_LEAF_NODE(DOT)
";" _NEW_LEAF_NODE(SEMI)
"," _NEW_LEAF_NODE(COMMA)
"=" _NEW_LEAF_NODE(ASSIGN)
"<" _NEW_LEAF_NODE(LT)
"<=" _NEW_LEAF_NODE(LE)
">" _NEW_LEAF_NODE(GT)
">=" _NEW_LEAF_NODE(GE)
"!=" _NEW_LEAF_NODE(NE)
"==" _NEW_LEAF_NODE(EQ)
"+" _NEW_LEAF_NODE(PLUS)
"-" _NEW_LEAF_NODE(MINUS)
"*" _NEW_LEAF_NODE(MUL)
"/" _NEW_LEAF_NODE(DIV)
"&&" _NEW_LEAF_NODE(AND)
"||" _NEW_LEAF_NODE(OR)
"!" _NEW_LEAF_NODE(NOT)
"(" _NEW_LEAF_NODE(LP)
")" _NEW_LEAF_NODE(RP)
"[" _NEW_LEAF_NODE(LB)
"]" _NEW_LEAF_NODE(RB)
"{" _NEW_LEAF_NODE(LC)
"}" _NEW_LEAF_NODE(RC)

{type} _NEW_LEAF_NODE(TYPE)

{int} _NEW_LEAF_NODE(INT)
{wrong_int} _ERROR_TYPE_A(yytext)

{float} _NEW_LEAF_NODE(FLOAT)

{char} _NEW_LEAF_NODE(CHAR)
{wrong_char} _ERROR_TYPE_A(yytext)

{id} _NEW_LEAF_NODE(ID)
{wrong_id} _ERROR_TYPE_A(yytext)

{single_line_comment} {}
{multi_line_comment} {}

{empty} {}

. { _ERROR_TYPE_A(yytext); return ERROR; }

%%

