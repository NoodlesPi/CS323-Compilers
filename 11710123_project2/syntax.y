%{
        #include "lex.yy.c"
        Node* root;
        int line_num = 1;
        FILE* fout = NULL;

        void yyerror(const char*);
%}

%union{
    class Node* Node;
}

%token <Node> STRUCT SEMI COMMA INT CHAR FLOAT
%token <Node> ADD SUB MUL DIV EQ ID TYPE DOT
%token <Node> ASSIGN AND OR LE LT GE GT NE NOT 
%token <Node> LC RC LB RB LP RP RETURN IF ELSE WHILE 

%type <Node> ExtDefList CompSt Stmt StmtList ExtDef ExtDecList
%type <Node> VarDec FunDec VarList Specifier StructSpecifier ParamDec


%type <Node> Args Exp DefList Def DecList Dec

%right ASSIGN NOT DOT
%left OR AND EQ NE LE LT GE GT
%left MUL ADD SUB DIV
%left LP RP LB RB

%nonassoc INT CHAR FLOAT ID ELSE
%%
Program:
        ExtDefList{root = new Node(Program, 1, "Program", $1->lineno, 1, $1);}
        ;

ExtDefList:
        ExtDef ExtDefList { $$ = new Node(ExtDefList, 1, "ExtDefList", $1->lineno, 2, $1, $2);}
        | {$$ = new Node(ExtDefList, 1, "ExtDefList", yylineno, 0);}
        ;

ExtDef:
        Specifier ExtDecList SEMI {$$ = new Node(ExtDef, 1, "ExtDef", $1->lineno, 3, $1, $2, $3);}
        | Specifier SEMI {$$ = new Node(ExtDef, 1, "ExtDef", $1->lineno, 2, $1, $2);}
        | Specifier FunDec CompSt { $$ = new Node(ExtDef, 1, "ExtDef", $1->lineno, 3, $1, $2, $3); }
        ;

ExtDecList:
        VarDec { $$ = new Node(ExtDecList, 1, "ExtDecList", $1->lineno, 1, $1);}
        | VarDec COMMA ExtDecList {$$ = new Node(ExtDecList, 1, "ExtDecList", $1->lineno, 3, $1, $2, $3);}
        ;


Specifier:
        TYPE {$$ = new Node(Specifier, 1, "Specifier", $1->lineno, 1, $1);}
        | StructSpecifier {$$ = new Node(Specifier, 1, "Specifier", $1->lineno, 1, $1);}
        ;

StructSpecifier:
        STRUCT ID LC DefList RC {$$ = new Node(StructSpecifier, 1, "StructSpecifier", $1->lineno, 5, $1, $2, $3, $4, $5);}
        | STRUCT ID {$$ = new Node(StructSpecifier, 1, "StructSpecifier", $1->lineno, 2, $1, $2);}
        ;

VarDec:
        VarDec LB INT RB {$$ = new Node(VarDec, 1, "VarDec", $1->lineno, 4, $1, $2, $3, $4);}
        | ID {$$ = new Node(VarDec, 1, "VarDec", $1->lineno, 1, $1);}   
        ;

FunDec:
        ID LP VarList RP { $$ = new Node(FunDec, 1, "FunDec", $1->lineno, 4, $1, $2, $3, $4);}
        | ID LP RP {$$ = new Node(FunDec, 1, "FunDec", $1->lineno, 3, $1, $2, $3); }
        ;

VarList:
        ParamDec COMMA VarList {$$ = new Node(VarList, 1, "VarList", $1->lineno, 3, $1, $2, $3);}
        | ParamDec { $$ = new Node(VarList, 1, "VarList", $1->lineno, 1, $1);}
        ;

ParamDec:
        Specifier VarDec {$$ = new Node(ParamDec, 1, "ParamDec", $1->lineno, 2, $1, $2);}
        ;


CompSt:
        LC DefList StmtList RC { $$ = new Node(CompSt, 1, "CompSt", $1->lineno, 4, $1, $2, $3, $4);}
        ;

StmtList:
        Stmt StmtList {$$ = new Node(StmtList, 1, "StmtList", $1->lineno, 2, $1, $2); }
        | {$$ = new Node(StmtList, 1, "StmtList", yylineno, 0);}
        ;

Stmt: 
        Exp SEMI {$$ = new Node(Stmt, 1, "Stmt", $1->lineno, 2, $1, $2);}
        | CompSt { $$ = new Node(Stmt, 1, "Stmt", $1->lineno, 1, $1);}
        | RETURN Exp SEMI { $$ = new Node(Stmt, 1, "Stmt", $1->lineno, 3, $1, $2, $3); }
        
        | IF LP Exp RP Stmt { $$ = new Node(Stmt, 1, "Stmt", $1->lineno, 5, $1, $2, $3, $4, $5);}
        | IF LP Exp RP Stmt ELSE Stmt {$$ = new Node(Stmt, 1, "Stmt", $1->lineno, 7, $1, $2, $3, $4, $5, $6, $7);}
        
        
        | WHILE LP Exp RP Stmt { $$ = new Node(Stmt, 1, "Stmt", $1->lineno, 5, $1, $2, $3, $4, $5);}
        ;



DefList:
        Def DefList { $$ = new Node(DefList, 1, "DefList", $1->lineno, 2, $1, $2);}
        |  {$$ = new Node(DefList, 1, "DefList", yylineno, 0); }
        ;

Def:
        Specifier DecList SEMI {$$ = new Node(Def, 1, "Def", $1->lineno, 3, $1, $2, $3); }
        ;

DecList:
        Dec { $$ = new Node(DecList, 1, "DecList", $1->lineno, 1, $1);}
        | Dec COMMA DecList {$$ = new Node(DecList, 1, "DecList", $1->lineno, 3, $1, $2, $3); }
        ;

Dec:
        VarDec {$$ = new Node(Dec, 1, "Dec", $1->lineno, 1, $1); }
        | VarDec ASSIGN Exp {$$ = new Node(Dec, 1, "Dec", $1->lineno, 3, $1, $2, $3); }
    ;

Exp:
        Exp ASSIGN Exp { $$ = new Node(Exp, 1, "Exp", $1->lineno, 3, $1, $2, $3); }
        | Exp AND Exp { $$ = new Node(Exp, 1, "Exp", $1->lineno, 3, $1, $2, $3); }
        | Exp OR Exp { $$ = new Node(Exp, 1, "Exp", $1->lineno, 3, $1, $2, $3); }
        | Exp LT Exp { $$ = new Node(Exp, 1, "Exp", $1->lineno, 3, $1, $2, $3); }
        | Exp LE Exp { $$ = new Node(Exp, 1, "Exp", $1->lineno, 3, $1, $2, $3); }
        | Exp GT Exp { $$ = new Node(Exp, 1, "Exp", $1->lineno, 3, $1, $2, $3); }
        | Exp GE Exp { $$ = new Node(Exp, 1, "Exp", $1->lineno, 3, $1, $2, $3); }
        | Exp NE Exp { $$ = new Node(Exp, 1, "Exp", $1->lineno, 3, $1, $2, $3); }
        | Exp EQ Exp { $$ = new Node(Exp, 1, "Exp", $1->lineno, 3, $1, $2, $3); }
        | Exp ADD Exp { $$ = new Node(Exp, 1, "Exp", $1->lineno, 3, $1, $2, $3); }
        | Exp SUB Exp { $$ = new Node(Exp, 1, "Exp", $1->lineno, 3, $1, $2, $3); }
        | Exp MUL Exp { $$ = new Node(Exp, 1, "Exp", $1->lineno, 3, $1, $2, $3); }
        | Exp DIV Exp { $$ = new Node(Exp, 1, "Exp", $1->lineno, 3, $1, $2, $3); }
        | LP Exp RP {$$ = $2; }
        | SUB Exp {$$ = new Node(Exp, 1, "Exp", $1->lineno, 2, $1, $2); }
        | NOT Exp {$$ = new Node(Exp, 1, "Exp", $1->lineno, 2, $1, $2);}   
        | ID LP Args RP {$$ = new Node(Exp, 1, "Exp", $1->lineno, 4, $1, $2, $3, $4);}
        | ID LP RP {$$ = new Node(Exp, 1, "Exp", $1->lineno, 3, $1, $2, $3);}    
        | Exp LB Exp RB {$$ = new Node(Exp, 1, "Exp", $1->lineno, 4, $1, $2, $3, $4);}
        | Exp DOT ID {$$ = new Node(Exp, 1, "Exp", $1->lineno, 3, $1, $2, $3);}
        | ID {$$ = new Node(Exp, 1, "Exp", $1->lineno, 1, $1); }
        | INT { $$ = new Node(Exp, 1, "Exp", $1->lineno, 1, $1); }
        | FLOAT { $$ = new Node(Exp, 1, "Exp", $1->lineno, 1, $1); }
        | CHAR { $$ = new Node(Exp, 1, "Exp", $1->lineno, 1, $1); }
        ;

Args: Exp COMMA Args {$$ = new Node(Args, 1, "Args", $1->lineno, 3, $1, $2, $3);}
        | Exp { $$ = new Node(Args, 1, "Args", $1->lineno, 1, $1);}
      
        ;

%%


void yyerror(const char *s){
    printf("syntax error: %s\n", s);
}

int main(int argc, char** argv){
    if(argc != 2) {
        fprintf(stderr, "Usage: %s <file_path>\n", argv[0]);
        exit(-1);
    }
    else if(yyin = fopen(argv[1], "r")) {
        char *tmp = (char*)malloc(sizeof(char)*(strlen(argv[1])+1));
        strcpy(tmp, argv[1]);
        int len = strlen(tmp);
        tmp[--len] = 't';
        tmp[--len] = 'u';
        tmp[--len] = 'o';
        
        fout = fopen(tmp, "w");
    }
    yyparse();
    ProcessProgram(root);
    return 0;
}