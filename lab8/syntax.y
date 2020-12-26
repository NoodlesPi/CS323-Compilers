%{
    #include <stdbool.h>
    #include"lex.yy.c"
    void yyerror(const char*);

    bool is_valid = true;  // use this value
%}
%union{
    class JsonObject* JsonObject;
    class ObjectMember* ObjectMember;
}


%token LC RC LB RB COLON COMMA
%token<JsonObject> STRING NUMBER
%token<JsonObject> TRUE FALSE VNULL
%type <JsonObject> Json Value Object Array Values;
%type <ObjectMember> Member Members;
%%

Json:
      Value{if(!checkValue($1)){is_valid = false;}}
    ;
Value:
      Object {$$ = $1;}
    | Array {$$ = $1;}
    | STRING {$$ = $1;}
    | NUMBER {$$ = $1;}
    | TRUE {$$ = $1;}
    | FALSE {$$ = $1;}
    | VNULL {$$ = $1;}
    ;
Object:
      LC RC {$$ = NewNull();}
    | LC Members RC{$$ = NewObjectFromMember($2);}
    ;
Members:
      Member {$$ = $1;}
    | Member COMMA Members {$$ = AddMemberItem($1, $3);}
    ;
Member:
      STRING COLON Value {$$ = NewMember($1, $3);}
    ;
Array:
      LB RB {$$ = NewArray(NewNull());}
    | LB Values RB {$$ = $2;}
    ;
Values:
      Value {$$ = NewArray($1);}
    | Value COMMA Values {$$ = AddArrayItem($1, $3);}
    ;
%%


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
    if(is_valid) {
        printf("%d\n", is_valid);
    }
    return 0;
}
