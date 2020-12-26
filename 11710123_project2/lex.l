%{
    #include "syntax.tab.h"
    #include "semantic.h"

    #define _NEW_LEAF_NODE(NAME, TYPE, SYMBOL_TYPE){\
        yylval.Node = new Node(TYPE, SYMBOL_TYPE, yytext, yylineno, 0) ;\
        return NAME;\
    }
    #define YY_DECL extern "C" int yylex()
%}
%option yylineno

number [-+]?[1-9][0-9]*|0
type int|float|char

id [a-zA-Z_][0-9a-zA-Z_]{0,31}

int {number}|0(x|X)([1-9a-fA-F][0-9a-fA-F]{0,7}|0)

float ({number}?\.[0-9]+)|({number}\.[0-9]*)

char '[ -~]'|'(\\x[0-9a-fA-F]{2})'

empty [ \n\r\t]

single_line_comment \/\/.*
multi_line_comment \/\*([^*]|[\r\n]|(\*+([^*\/]|[\r\n])))*\*+\/

%%
[\t\n] ;

"return" {_NEW_LEAF_NODE(RETURN, T_RETURN, 3); }
"if" {_NEW_LEAF_NODE(IF, T_IF, 3); }
"else" {_NEW_LEAF_NODE(ELSE, T_ELSE, 3); }
"while" {_NEW_LEAF_NODE(WHILE, T_WHILE, 3); }
"struct" {_NEW_LEAF_NODE(STRUCT, T_STRUCT, 3); }

"," {_NEW_LEAF_NODE(COMMA, T_COMMA, 3); }
";" {_NEW_LEAF_NODE(SEMI, T_SEMI, 3); }
"{" {_NEW_LEAF_NODE(LC, T_LC, 3);}
"}" {_NEW_LEAF_NODE(RC, T_RC, 3);}
"<" {_NEW_LEAF_NODE(LT, T_LT, 3);}
">=" {_NEW_LEAF_NODE(GE, T_GE, 3);}
">" {_NEW_LEAF_NODE(GT, T_GT, 3);}
"!=" {_NEW_LEAF_NODE(NE, T_NE, 3);}
"+" {_NEW_LEAF_NODE(ADD, T_ADD, 3);}
"-" {_NEW_LEAF_NODE(SUB, T_SUB, 3);}
"*" {_NEW_LEAF_NODE(MUL, T_MUL, 3);}
"/" {_NEW_LEAF_NODE(DIV, T_DIV, 3);}
"." {_NEW_LEAF_NODE(DOT, T_DOT, 3);}
"[" {_NEW_LEAF_NODE(LB, T_LB, 3);}
"]" {_NEW_LEAF_NODE(RB, T_RB, 3);}
"(" {_NEW_LEAF_NODE(LP, T_LP, 3); }
")" {_NEW_LEAF_NODE(RP, T_RP, 3);}
"&&" {_NEW_LEAF_NODE(AND, T_AND, 3);}
"||" {_NEW_LEAF_NODE(OR, T_OR, 3);}
"<=" {_NEW_LEAF_NODE(LE, T_LE, 3);}
"==" {_NEW_LEAF_NODE(EQ, T_EQ, 3);}
"=" {_NEW_LEAF_NODE(ASSIGN, T_ASSIGN, 3);}
"!" {_NEW_LEAF_NODE(NOT, T_NOT, 3);}


{type} { _NEW_LEAF_NODE(TYPE, T_TYPE, 2);}

{float} {_NEW_LEAF_NODE(FLOAT, T_FLOAT, 2);}
{char} {_NEW_LEAF_NODE(CHAR, T_CHAR, 2); }
{int} {_NEW_LEAF_NODE(INT, T_INT, 2); }
{id} {_NEW_LEAF_NODE(ID, T_ID, 2);}

{single_line_comment} {}
{multi_line_comment} {}

{empty} {}
. {}

%%
