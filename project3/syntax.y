%{
    #include"lex.yy.c"

    FILE *fout = NULL;

    #define _MISSING_SEMI_ERROR(e) { \
        has_error = 1; \
        fprintf(fout, "Error type B at Line %d: Missing semicolon ';'\n", e->lineno); \
    }

    #define _MISSING_RP_ERROR(e) { \
        has_error = 1; \
        fprintf(fout, "Error type B at Line %d: Missing closing parenthesis ')'\n", e->lineno); \
    }

    #define _MISSING_Spec_ERROR(e) { \
        has_error = 1; \
        fprintf(fout, "Error type B at Line %d: Missing specifier \n", e->lineno); \
    }

    #define _INVALID_INDEX_ERROR(e) { \
        has_error = 1; \
        fprintf(fout, "Error type B at Line %d: Invalid index \n", e->lineno); \
    }
    short has_error = 0;

    void yyerror(const char*);
%}

%union {
    Node* node;
}

%token <node> INT FLOAT CHAR 
%token <node> ID TYPE STRUCT IF ELSE WHILE RETURN
%token <node> DOT SEMI COMMA ASSIGN LT LE GT GE NE EQ 
%token <node> PLUS MINUS MUL DIV AND OR NOT LP RP LB RB LC RC ERROR

%type <node> Program ExtDefList ExtDef ExtDecList;
%type <node> Specifier StructSpecifier;
%type <node> VarDec FunDec VarList ParamDec;
%type <node> CompSt StmtList Stmt;
%type <node> DefList Def DecList Dec;
%type <node> Exp Args;

%right ASSIGN
%right NOT

%left OR
%left AND
%left LT LE GT GE NE EQ
%left PLUS MINUS
%left MUL DIV
%left COMMA DOT

%nonassoc LP RP LB RB LC RC
%nonassoc SEMI
%nonassoc ELSE
%% 

/* high-level definition */
Program: 
    ExtDefList {
        $$ = newInternalNode("Program", 1, $1);
        if (!has_error) printSyntaxTree($$, 0);
    }
    ;
ExtDefList:
    ExtDef ExtDefList { $$ = newInternalNode("ExtDefList", 2, $1, $2); }
    | %empty { $$ = newInternalNode("Empty", -1); }
    ;
ExtDef:
    Specifier ExtDecList SEMI { $$ = newInternalNode("ExtDef", 3, $1, $2, $3); }
    | Specifier SEMI { $$ = newInternalNode("ExtDef", 2, $1, $2); }
    | Specifier FunDec CompSt { $$ = newInternalNode("ExtDef", 3, $1, $2, $3); }
    ;
ExtDecList:
    VarDec { $$ = newInternalNode("ExtDecList", 1, $1); }
    | VarDec COMMA ExtDecList { $$ = newInternalNode("ExtDecList", 3, $1, $2, $3); }
    ;

/* specifier */
Specifier:
    TYPE { $$ = newInternalNode("Specifier", 1, $1); }
    | StructSpecifier { $$ = newInternalNode("Specifier", 1, $1); }
    ;
StructSpecifier:
    STRUCT ID LC DefList RC { $$ = newInternalNode("StructSpecifier", 5, $1, $2, $3, $4, $5); }
    | STRUCT ID { $$ = newInternalNode("StructSpecifier", 2, $1, $2); }
    ;

/* declarator */
VarDec:
    ID { $$ = newInternalNode("VarDec", 1, $1); }
    | INT {
        has_error = 1;
        _ERROR_TYPE_A(yytext);
        $$ = newInternalNode("VarDec", 1, $1);
    }
    | ERROR {
        has_error = 1;
        $$ = newInternalNode("VarDec", 1, $1);
    }
    | VarDec LB INT RB { $$ = newInternalNode("VarDec", 4, $1, $2, $3, $4); }
    | VarDec LB TYPE RB { 
        _INVALID_INDEX_ERROR($2);
        $$ = newInternalNode("VarDec", 4, $1, $2, $3, $4);
    }
    ;
FunDec:
    ID LP VarList RP { $$ = newInternalNode("FunDec", 4, $1, $2, $3, $4); }
    | ID LP RP { $$ = newInternalNode("FunDec", 3, $1, $2, $3); }
    | ID LP error {
        $$ = newInternalNode("FunDec", 2, $1, $2);
        _MISSING_RP_ERROR($2);
    }
    ;
VarList:
    ParamDec COMMA VarList { $$ = newInternalNode("VarList", 3, $1, $2, $3); }
    | ParamDec { $$ = newInternalNode("VarList", 1, $1); }
    ;
ParamDec:
    Specifier VarDec { $$ = newInternalNode("ParamDec", 2, $1, $2); }
    | VarDec {
        _MISSING_Spec_ERROR($1);
        $$ = newInternalNode("ParamDec", 1, $1);
    }
    ;

/* statement */
CompSt:
    LC DefList StmtList RC { $$ = newInternalNode("CompSt", 4, $1, $2, $3, $4); }
    ;
StmtList:
    Stmt StmtList { $$ = newInternalNode("StmtList", 2, $1, $2); }
    | %empty { $$ = newInternalNode("Empty", -1); }
    ;
Stmt:
    Exp SEMI { $$ = newInternalNode("Stmt", 2, $1, $2); }
    | CompSt { $$ = newInternalNode("Stmt", 1, $1); }
    | RETURN Exp SEMI { $$ = newInternalNode("Stmt", 3, $1, $2, $3); }
    | RETURN error SEMI {
        has_error = 1;
        $$ = newInternalNode("Stmt", 2, $1, $3);
    }
    | RETURN Exp error { 
        _MISSING_SEMI_ERROR($2);
        $$ = newInternalNode("Stmt", 2, $1, $2); }
    | IF LP Exp RP Stmt { $$ = newInternalNode("Stmt", 5, $1, $2, $3, $4, $5); }
    | IF LP Exp RP Stmt ELSE Stmt { $$ = newInternalNode("Stmt", 7, $1, $2, $3, $4, $5, $6, $7); }
    | WHILE LP Exp RP Stmt { $$ = newInternalNode("Stmt", 5, $1, $2, $3, $4, $5); }
    ;

/* local definition */
DefList:
    Def DefList { $$ = newInternalNode("DefList", 2, $1, $2); }
    | %empty { $$ = newInternalNode("Empty", -1); }
    ;
Def:
    Specifier DecList error { 
        _MISSING_SEMI_ERROR($2);
        $$ = newInternalNode("Def", 3, $1, $2);}
    | Specifier DecList SEMI { $$ = newInternalNode("Def", 3, $1, $2, $3); }
    | error DecList SEMI {
        has_error = 1;
        fprintf(fout, "Error type B at Line %d: Missing specifier \n", $$->lineno - 1);
        $$ = newInternalNode("Def", 2, $2, $3);
    }
    | Specifier error SEMI {
        has_error = 1;
        $$ = newInternalNode("Def", 2, $1, $3);
    }
    ;
DecList:
    Dec { $$ = newInternalNode("DecList", 1, $1); }
    | Dec COMMA DecList { $$ = newInternalNode("DecList", 3, $1, $2, $3); }
    ;
Dec:
    VarDec { $$ = newInternalNode("Dec", 1, $1); }
    | VarDec ASSIGN Exp { $$ = newInternalNode("Dec", 3, $1, $2, $3); }
    ;

/* Expression */
Exp:
    Exp ASSIGN Exp { $$ = newInternalNode("Exp", 3, $1, $2, $3); }
    | Exp AND Exp { $$ = newInternalNode("Exp", 3, $1, $2, $3); }
    | Exp OR Exp { $$ = newInternalNode("Exp", 3, $1, $2, $3); }
    | Exp LT Exp { $$ = newInternalNode("Exp", 3, $1, $2, $3); }
    | Exp LE Exp { $$ = newInternalNode("Exp", 3, $1, $2, $3); }
    | Exp GT Exp { $$ = newInternalNode("Exp", 3, $1, $2, $3); }
    | Exp GE Exp { $$ = newInternalNode("Exp", 3, $1, $2, $3); }
    | Exp NE Exp { $$ = newInternalNode("Exp", 3, $1, $2, $3); }
    | Exp EQ Exp { $$ = newInternalNode("Exp", 3, $1, $2, $3); }
    | Exp PLUS Exp { $$ = newInternalNode("Exp", 3, $1, $2, $3); }
    | Exp MINUS Exp { $$ = newInternalNode("Exp", 3, $1, $2, $3); }
    | Exp MUL Exp { $$ = newInternalNode("Exp", 3, $1, $2, $3); }
    | Exp DIV Exp { $$ = newInternalNode("Exp", 3, $1, $2, $3); }
    | LP Exp RP { $$ = newInternalNode("Exp", 3, $1, $2, $3); }
    | MINUS Exp  { $$ = newInternalNode("Exp", 2, $1, $2); }
    | NOT Exp  { $$ = newInternalNode("Exp", 2, $1, $2); }
    | ID LP Args RP { $$ = newInternalNode("Exp", 4, $1, $2, $3, $4); }
    | ID LP Args error {
        _MISSING_RP_ERROR($1);
        $$ = newInternalNode("Exp", 4, $1, $2, $3);
    }
    | ID LP RP { $$ = newInternalNode("Exp", 3, $1, $2, $3); }
    | Exp LB Exp RB { $$ = newInternalNode("Exp", 4, $1, $2, $3, $4); }
    | Exp DOT ID { $$ = newInternalNode("Exp", 3, $1, $2, $3); }
    | ID  { $$ = newInternalNode("Exp", 1, $1); }
    | INT { $$ = newInternalNode("Exp", 1, $1); }
    | FLOAT{ $$ = newInternalNode("Exp", 1, $1); }
    | CHAR{ $$ = newInternalNode("Exp", 1, $1); }
    | Exp ERROR Exp {
        has_error = 1;
        $$ = newInternalNode("Exp", 3, $1, $2, $3);
    }
    | ERROR {
        has_error = 1;
        $$ = newInternalNode("Exp", 1, $1);
    }
    ;
Args:
    Exp COMMA Args { $$ = newInternalNode("Args", 3, $1, $2, $3); }
    | Exp { $$ = newInternalNode("Args", 1, $1); }
    ;

%%

/* void yyerror(const char *s){
    printf("syntax error: %s\n", s);
} */

int main(int argc, char **argv){
    if(argc != 2) {
        fprintf(stderr, "Usage: %s <file_path>\n", argv[0]);
        exit(-1);
    }
    else if(yyin = fopen(argv[1], "r")) {
        char *tmp = (char*)malloc(sizeof(char)*strlen(argv[1]));
        strcpy(tmp, argv[1]);
        int len = strlen(tmp);
        tmp[--len] = 't';
        tmp[--len] = 'u';
        tmp[--len] = 'o';
        
        fout = fopen(tmp, "w");
    }
    yyparse();
    return 0;
}