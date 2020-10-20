%{
    #include"lex.yy.c"
    void yyerror(const char*);
%}

%token LC RC LB RB COLON COMMA
%token STRING NUMBER
%token TRUE FALSE VNULL
%%

Json:
      Value
    ;
Value:
      Object
    | Array
    | STRING
    | NUMBER
    | TRUE
    | FALSE
    | VNULL
    | NUMBER NUMBER error { puts("numbers cannot have leading zeroes, recovered"); }
    ;
Object:
      LC RC
    | LC Members RC
    | LC Members RC error { puts("misplaced quoted value, recovered");}
    ;
Members:
      Member
    | Member COMMA Members
    ;
Member:
      STRING COLON Value
    | STRING COLON COLON Value error { puts("double colon, recovered"); }
    | STRING COMMA Value error { puts("comma instead of colon, recovered"); }
    | STRING Value error { puts("missing colon, recovered"); }
    | STRING COLON Value COMMA error { puts("extra comma, recovered"); }
    ;
Array:
      LB RB
    | LB Values RB
    | LB Values error { puts("unclosed array, recovered"); }
    | LB Values RC error { puts("unmatched right bracket, recovered"); }
    | LB Values RB COMMA error { puts("comma after the close, recovered"); }
    | LB Values RB RB error { puts("extra close, recovered"); }
    ;
Values:
      Value
    | COMMA Values error { puts("missing value, recovered"); }
    | Value COMMA Values
    | Value COMMA error { puts("extra comma, recovered"); }
    | Value COMMA COMMA error { puts("double extra comma, recovered"); }
    ;
%%

void yyerror(const char *s){
    printf("syntax error: ");
}

int main(int argc, char **argv){
    if(argc != 2) {
        fprintf(stderr, "Usage: %s <file_path>\n", argv[0]);
        exit(-1);
    }
    else if(!(yyin = fopen(argv[1], "r"))) {
        perror(argv[1]);
        exit(-1);
    }
    yyparse();
    return 0;
}
