%{
    #include "tree.hpp"
    #include "syntax.tab.h"

    #define _NEW_NODE(name, value) { \
        yylval.node = new Node(#name, value, yylineno, 0); \
        return name; \
    }

    #define YY_DECL extern "C" int yylex()

%}
%option yylineno

number [1-9][0-9]*|0
type int|float|char

id [a-zA-Z_][0-9a-zA-Z_]{0,31}

int {number}|0(x|X)([1-9a-fA-F][0-9a-fA-F]{0,7}|0)

float ({number}?\.[0-9]+)|({number}\.[0-9]*)

char '[ -~]'|'(\\x[0-9a-fA-F]{2})'

empty [ \n\r\t]

single_line_comment \/\/.*
multi_line_comment \/\*([^*]|[\r\n]|(\*+([^*\/]|[\r\n])))*\*+\/
%%

"struct" _NEW_NODE(STRUCT, "")
"if" _NEW_NODE(IF, "")
"else" _NEW_NODE(ELSE, "")
"while" _NEW_NODE(WHILE, "")
"return" _NEW_NODE(RETURN, "")
"read" _NEW_NODE(READ, "")
"write" _NEW_NODE(WRITE, "")
"." _NEW_NODE(DOT, ".")
";" _NEW_NODE(SEMI, ";")
"," _NEW_NODE(COMMA, ",")
"=" _NEW_NODE(ASSIGN, "=")
"<" _NEW_NODE(LT, "<")
"<=" _NEW_NODE(LE, "<=")
">" _NEW_NODE(GT, ">")
">=" _NEW_NODE(GE, ">=")
"!=" _NEW_NODE(NE, "!=")
"==" _NEW_NODE(EQ, "==")
"+" _NEW_NODE(PLUS, "+")
"-" _NEW_NODE(MINUS, "-")
"*" _NEW_NODE(MUL, "*")
"/" _NEW_NODE(DIV, "/")
"&&" _NEW_NODE(AND, "&&")
"||" _NEW_NODE(OR, "||")
"!" _NEW_NODE(NOT, "!")
"(" _NEW_NODE(LP, "")
")" _NEW_NODE(RP, "")
"[" _NEW_NODE(LB, "")
"]" _NEW_NODE(RB, "")
"{" _NEW_NODE(LC, "")
"}" _NEW_NODE(RC, "")

{type} _NEW_NODE(TYPE, yytext)

{int} _NEW_NODE(INT, yytext)

{float} _NEW_NODE(FLOAT, yytext)

{char} _NEW_NODE(CHAR, yytext)

{id} _NEW_NODE(ID, yytext)

{single_line_comment} {}
{multi_line_comment} {}

{empty} {}

. {}

%%

