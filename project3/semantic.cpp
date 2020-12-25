#include "semantic.hpp"

multimap<string, Variable *> var_map;
multimap<string, Structure *> strc_map;

int global_scope_level;

Type *EMPTY_TYPE;

// init all relative variables
void init(){
    var_map = multimap<string, Variable*>();
    strc_map = multimap<string, Structure*>();
    global_scope_level = 0;
    EMPTY_TYPE = new Type();;
}

Primitive *handle_Type(Node *node){
   return new Primitive(node->token);
}

/*
    Exp COMMA Args { $$ = new Node("Args", "", $1->line_num, 3, $1, $2, $3); }
    | Exp { $$ = new Node("Args", "", $1->line_num, 1, $1); }
    ;
*/
// return args list
vector<Type *> handle_Args(Node *node){
    vector<Type *> args = vector<Type *>();
    args.push_back(handle_Exp(node->child_list[0]));
    Node *tmp = node;
    while(tmp->child_num > 1){
        tmp = tmp->child_list[2];
        args.push_back(handle_Exp(node->child_list[0]));
    }
    return args;
}

/*
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
*/
Type* handle_Exp(Node *node, bool single=false){
    /*
    | ID LP Args RP { $$ = new Node("Exp", "", $1->line_num, 4, $1, $2, $3, $4); }
    | ID LP RP { $$ = new Node("Exp", "", $1->line_num, 3, $1, $2, $3); }
    | ID  { $$ = new Node("Exp", "", $1->line_num, 1, $1); }
    */
   if(node->child_list[0]->token == "ID"){
       string name = node->value;
       vector<Type *> args = vector<Type *>();

       bool is_func = false;
       if(node->child_num > 1){
           if(node->child_num > 3){
               args = handle_Args(node->child_list[2]);
               is_func = true;
           }
       }

       return var_map.find(name)->second->type;
   }

   /*
       | INT { $$ = new Node("Exp", "", $1->line_num, 1, $1); }
    | FLOAT{ $$ = new Node("Exp", "", $1->line_num, 1, $1); }
    | CHAR{ $$ = new Node("Exp", "", $1->line_num, 1, $1); }
    */
   if(node->child_list[0]->token != "ID" && node->child_num==1){
       return new Primitive(node->child_list[0]->token);
   }

   /*
    | MINUS Exp  { $$ = new Node("Exp", "", $1->line_num, 2, $1, $2); }
    | NOT Exp  { $$ = new Node("Exp", "", $1->line_num, 2, $1, $2); }
   */
    if(node->child_list[0]->token == "NOT" || node->child_list[0]->token == "MINUS"){
        return handle_Exp(node->child_list[1]);
    }

    /*
    Exp DOT ID { $$ = new Node("Exp", "", $1->line_num, 3, $1, $2, $3); }
    */
   if(node->child_list[1]->token == "DOT"){
       return handle_Exp(node->child_list[0]);
   }

   /*
   ID LP Args RP { $$ = new Node("Exp", "", $1->line_num, 4, $1, $2, $3, $4); }
   */
   if(node->child_list[1]->token == "LB"){
        Type *type = handle_Exp(node->child_list[0]);
        return dynamic_cast<Array *>(type)->type;
   }

    /*
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
    */
    if(single && node->child_list[1]->token != "ASSIGN")return EMPTY_TYPE;
    
    Type *left = handle_Exp(node->child_list[0], node->child_list[1]->token == "ASSIGN");
    Type *right = handle_Exp(node->child_list[2]);

    if(*right==*EMPTY_TYPE || *left==*EMPTY_TYPE) return EMPTY_TYPE;

    return left;
}
/*
VarDec:
    ID { $$ = new Node("VarDec", "", $1->line_num, 1, $1); }
    | VarDec LB INT RB { $$ = new Node("VarDec", "", $1->line_num, 4, $1, $2, $3, $4); }
    ;
*/
// return a variable
Variable* handle_VarDec(Node *node, Type *type){
    // use a stack to simulate recursion
    vector<Node *> stack = vector<Node *>();
    stack.push_back(node);
    string name;
    while(!stack.empty()){
        Node *node = stack.back();
        // handle ID
        if(node->token == "ID"){
            stack.pop_back();
            name = node->child_list[0]->value;
            while(!stack.empty()){
                node = stack.back();
                type = new Array(type, atoi(node->child_list[2]->value.c_str()));
                stack.pop_back();
            }
        }else{
            stack.push_back(node->child_list[0]);
        }
    }
    
    return new Variable(name, type, {}, 0 , global_scope_level, node->line_num);
}

/*
Dec:
    VarDec { $$ = new Node("Dec", "", $1->line_num, 1, $1); }
    | VarDec ASSIGN Exp { $$ = new Node("Dec", "", $1->line_num, 3, $1, $2, $3); }
    ;
*/
// 返回定义的类型
Variable* handle_Dec(Node *node, Type *type){
    Variable *var = handle_VarDec(node->child_list[0], type);
    type = var->type;
    if(node->child_num > 1){
        Type *exp = handle_Exp(node->child_list[2]);
    }
    return var;
}
/*
DecList:
    Dec { $$ = new Node("DecList", "", $1->line_num, 1, $1); }
    | Dec COMMA DecList { $$ = new Node("DecList", "", $1->line_num, 3, $1, $2, $3); }
    ;
*/
vector<Variable *> handle_DecList(Node *node, Type *type){
    vector<Variable *> l = vector<Variable *>();
    l.push_back(handle_Dec(node->child_list[0], type));
}
/*
Def:
    Specifier DecList SEMI { $$ = new Node("Def", "", $1->line_num, 3, $1, $2, $3); }
    ;
*/
vector<Variable *> handle_Def(Node *node){
    Type *type = handle_Specifier(node->child_list[0]);
    
}
/*
DefList:
    Def DefList { $$ = new Node("DefList", "", $1->line_num, 2, $1, $2); }
    | %empty { $$ = new Node("Empty", "", yylineno, 0); }
    ;
*/
vector<Variable *> handle_DefList(Node *node){
    vector<Variable *> l = vector<Variable *>();
    Node *tmp = node;
    while(tmp->child_num){
        auto def = handle_Def(tmp->child_list[0]);
    }
}
/*
StructSpecifier:
    STRUCT ID LC DefList RC { $$ = new Node("StructSpecifier", "", $1->line_num, 5, $1, $2, $3, $4, $5); }
    | STRUCT ID { $$ = new Node("StructSpecifier", "", $1->line_num, 2, $1, $2); }
    ;
*/
Type *handle_StructSpecifier(Node *node){
    string name = node->value;
    if(node->child_num > 3){
        Structure *struc = new Structure(name, handle_DefList(node->child_list[3]))
    }else{

    }
}
/*
Specifier:
    TYPE { $$ = new Node("Specifier", "", $1->line_num, 1, $1); }
    | StructSpecifier { $$ = new Node("Specifier", "", $1->line_num, 1, $1); }
    ;
*/
Type *handle_Specifier(Node *node){
    if(node->token == "TYPE"){
        return handle_Type(node->child_list[0]);
    }
    return handle_StructSpecifier(node->child_list[0]);
}

/*
ExtDef:
    Specifier ExtDecList SEMI { $$ = new Node("ExtDef", "", $1->line_num, 3, $1, $2, $3); }
    | Specifier SEMI { $$ = new Node("ExtDef", "", $1->line_num, 2, $1, $2); }
    | Specifier FunDec CompSt { $$ = new Node("ExtDef", "", $1->line_num, 3, $1, $2, $3); }
    ;
*/
void handle_ExtDef(Node *node){
    // return type int, float, char or struct
    Type *t = handle_Specifier(node->child_list[0]);
}
/*
ExtDefList:
    ExtDef ExtDefList { $$ = new Node("ExtDefList", "", $1->line_num, 2, $1, $2); }
    | %empty { $$ = new Node("ExtDefList", "", yylineno, 0); }
    ;
*/
void handle_ExtDefList(Node *node){
    Node *tmp = node;
    while(tmp->child_num){
        handle_ExtDef(tmp->child_list[0]);
        tmp = tmp->child_list[1];
    }
}

/*
Program: 
ExtDefList { root = new Node("Program", "", $1->line_num, 1, $1); }
;
 */
// semantic opereations enterance
void handle_Program(Node *root){
    init();
    handle_ExtDefList(root->child_list[0]);
}