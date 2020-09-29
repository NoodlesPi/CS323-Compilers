%{
    #include"lex.yy.c"
    void yyerror(const char *s){}
    int result;
%}
%token LP RP LB RB LC RC
%%
String: %empty
| LP String RP String
| LB String RB String
| LC String RC String
%%

int validParentheses(char *expr){
    yy_scan_string(expr);
    result = yyparse();
    return !result;
}
