%{

    #include"syntax.tab.h"
    #include "JSONType.h"
    void yyerror(const char* s){}
    

    #define YY_DECL extern "C" int yylex()
%}
%option noyywrap

unic u[0-9a-fA-F]{4}
esc \\([\"\\/bfnrt]|{unic})
scp [^"\\\x00-\x1f]
string \"({esc}|{scp})*\"

int 0|[1-9][0-9]*
frac \.[0-9]+
exp [Ee][+-]?[0-9]+
number -?{int}{frac}?{exp}?

empty [ \n\r\t]

%%

"{" { return LC; }
"}" { return RC; }
"[" { return LB; }
"]" { return RB; }
":" { return COLON; }
"," { return COMMA; }

"true" { yylval.JsonObject= NewBoolean(1); return TRUE; }
"false" { yylval.JsonObject = NewBoolean(0);return FALSE; }
"null" { yylval.JsonObject = NewNull();return VNULL; }
{string} { yylval.JsonObject = NewString(yytext); return STRING; }
{number} { yylval.JsonObject = NewNumber(yytext); return NUMBER; }


{empty} {}
