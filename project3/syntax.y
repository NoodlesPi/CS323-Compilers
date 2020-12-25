%{
    #include"lex.yy.c"

    FILE *fout = NULL;
    Node *root;

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
    class Node *node;
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
    ExtDefList { root = new Node("Program", "", $1->line_num, 1, $1); }
    ;
ExtDefList:
    ExtDef ExtDefList { $$ = new Node("ExtDefList", "", $1->line_num, 2, $1, $2); }
    | %empty { $$ = new Node("ExtDefList", "", yylineno, 0); }
    ;
ExtDef:
    Specifier ExtDecList SEMI { $$ = new Node("ExtDef", "", $1->line_num, 3, $1, $2, $3); }
    | Specifier SEMI { $$ = new Node("ExtDef", "", $1->line_num, 2, $1, $2); }
    | Specifier FunDec CompSt { $$ = new Node("ExtDef", "", $1->line_num, 3, $1, $2, $3); }
    ;
ExtDecList:
    VarDec { $$ = new Node("ExtDecList", "", $1->line_num, 1, $1); }
    | VarDec COMMA ExtDecList { $$ = new Node("ExtDecList", "", $1->line_num, 3, $1, $2, $3); }
    ;

/* specifier */
Specifier:
    TYPE { $$ = new Node("Specifier", "", $1->line_num, 1, $1); }
    | StructSpecifier { $$ = new Node("Specifier", "", $1->line_num, 1, $1); }
    ;
StructSpecifier:
    STRUCT ID LC DefList RC { $$ = new Node("StructSpecifier", "", $1->line_num, 5, $1, $2, $3, $4, $5); }
    | STRUCT ID { $$ = new Node("StructSpecifier", "", $1->line_num, 2, $1, $2); }
    ;

/* declarator */
VarDec:
    ID { $$ = new Node("VarDec", "", $1->line_num, 1, $1); }
    | VarDec LB INT RB { $$ = new Node("VarDec", "", $1->line_num, 4, $1, $2, $3, $4); }
    ;
FunDec:
    ID LP VarList RP { $$ = new Node("FunDec", "", $1->line_num, 4, $1, $2, $3, $4); }
    | ID LP RP { $$ = new Node("FunDec", "", $1->line_num, 3, $1, $2, $3); }
    ;
VarList:
    ParamDec COMMA VarList { $$ = new Node("VarList", "", $1->line_num, 3, $1, $2, $3); }
    | ParamDec { $$ = new Node("VarList", "", $1->line_num, 1, $1); }
    ;
ParamDec:
    Specifier VarDec { $$ = new Node("ParamDec", "", $1->line_num, 2, $1, $2); }
    ;

/* statement */
CompSt:
    LC DefList StmtList RC { $$ = new Node("CompSt", "", $1->line_num, 4, $1, $2, $3, $4); }
    ;
StmtList:
    Stmt StmtList { $$ = new Node("StmtList", "", $1->line_num, 2, $1, $2); }
    | %empty { $$ = new Node("Empty", "", yylineno, 0); }
    ;
Stmt:
    Exp SEMI { $$ = new Node("Stmt", "", $1->line_num, 2, $1, $2); }
    | CompSt { $$ = new Node("Stmt", "", $1->line_num, 1, $1); }
    | RETURN Exp SEMI { $$ = new Node("Stmt", "", $1->line_num, 3, $1, $2, $3); }

    | IF LP Exp RP Stmt { $$ = new Node("Stmt", "", $1->line_num, 5, $1, $2, $3, $4, $5); }
    | IF LP Exp RP Stmt ELSE Stmt { $$ = new Node("Stmt", "", $1->line_num, 7, $1, $2, $3, $4, $5, $6, $7); }
    | WHILE LP Exp RP Stmt { $$ = new Node("Stmt", "", $1->line_num, 5, $1, $2, $3, $4, $5); }
    ;

/* local definition */
DefList:
    Def DefList { $$ = new Node("DefList", "", $1->line_num, 2, $1, $2); }
    | %empty { $$ = new Node("Empty", "", yylineno, 0); }
    ;
Def:
    Specifier DecList SEMI { $$ = new Node("Def", "", $1->line_num, 3, $1, $2, $3); }
    ;
DecList:
    Dec { $$ = new Node("DecList", "", $1->line_num, 1, $1); }
    | Dec COMMA DecList { $$ = new Node("DecList", "", $1->line_num, 3, $1, $2, $3); }
    ;
Dec:
    VarDec { $$ = new Node("Dec", "", $1->line_num, 1, $1); }
    | VarDec ASSIGN Exp { $$ = new Node("Dec", "", $1->line_num, 3, $1, $2, $3); }
    ;

/* Expression */
Exp:
    Exp ASSIGN Exp { $$ = new Node("Exp", "", $1->line_num, 3, $1, $2, $3); }
    | Exp AND Exp { $$ = new Node("Exp", "", $1->line_num, 3, $1, $2, $3); }
    | Exp OR Exp { $$ = new Node("Exp", "", $1->line_num, 3, $1, $2, $3); }
    | Exp LT Exp { $$ = new Node("Exp", "", $1->line_num, 3, $1, $2, $3); }
    | Exp LE Exp { $$ = new Node("Exp", "", $1->line_num, 3, $1, $2, $3); }
    | Exp GT Exp { $$ = new Node("Exp", "", $1->line_num, 3, $1, $2, $3); }
    | Exp GE Exp { $$ = new Node("Exp", "", $1->line_num, 3, $1, $2, $3); }
    | Exp NE Exp { $$ = new Node("Exp", "", $1->line_num, 3, $1, $2, $3); }
    | Exp EQ Exp { $$ = new Node("Exp", "", $1->line_num, 3, $1, $2, $3); }
    | Exp PLUS Exp { $$ = new Node("Exp", "", $1->line_num, 3, $1, $2, $3); }
    | Exp MINUS Exp { $$ = new Node("Exp", "", $1->line_num, 3, $1, $2, $3); }
    | Exp MUL Exp { $$ = new Node("Exp", "", $1->line_num, 3, $1, $2, $3); }
    | Exp DIV Exp { $$ = new Node("Exp", "", $1->line_num, 3, $1, $2, $3); }
    
    | LP Exp RP { $$ = new Node("Exp", "", $1->line_num, 3, $1, $2, $3); }
    | MINUS Exp  { $$ = new Node("Exp", "", $1->line_num, 2, $1, $2); }
    | NOT Exp  { $$ = new Node("Exp", "", $1->line_num, 2, $1, $2); }
    
    | ID LP Args RP { $$ = new Node("Exp", "", $1->line_num, 4, $1, $2, $3, $4); }
    | ID LP RP { $$ = new Node("Exp", "", $1->line_num, 3, $1, $2, $3); }
    
    | Exp LB Exp RB { $$ = new Node("Exp", "", $1->line_num, 4, $1, $2, $3, $4); }
    | Exp DOT ID { $$ = new Node("Exp", "", $1->line_num, 3, $1, $2, $3); }
    
    | ID  { $$ = new Node("Exp", "", $1->line_num, 1, $1); }
    | INT { $$ = new Node("Exp", "", $1->line_num, 1, $1); }
    | FLOAT{ $$ = new Node("Exp", "", $1->line_num, 1, $1); }
    | CHAR{ $$ = new Node("Exp", "", $1->line_num, 1, $1); }
    ;
Args:
    Exp COMMA Args { $$ = new Node("Args", "", $1->line_num, 3, $1, $2, $3); }
    | Exp { $$ = new Node("Args", "", $1->line_num, 1, $1); }
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

    // 假设无错误
    fclose(fout);
    return 0;
}