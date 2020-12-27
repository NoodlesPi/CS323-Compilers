#include "semantic.hpp"

int global_scope_level;

multimap<string, Variable *> var_map;
multimap<string, Structure *> strc_map;

Type *EMPTY_TYPE;

Type::Type(int scope_level=0, int line_num=1){
    this->scope_level = scope_level;
    this->line_num = line_num;
}

Primitive::Primitive(string token, int scope_level, int line_num){
    this->token = token;
    Type::scope_level = scope_level;
    Type::line_num = line_num;
}

Array::Array(Type *type, int size, int scope_level=0, int line_num=1){
    this->type = type;
    this->size = size;
    Type::scope_level = scope_level;
    Type::line_num = line_num;
}

Variable::Variable(string name, Type *type, vector<Variable *> l = {}, int is_func=0, int scope_level = 0, int line_num = 1){
    this->name = name;
    this->type = type;
    this->is_func = is_func;
    this->line_num = line_num;
    this->scope_level = scope_level;
    this->args = vector<Variable *>();

    for(auto it : l){
        this->args.push_back(it);
    }
}
int Variable::get_size(){
    return type_size;
}

Structure::Structure(string name, vector<Variable *> fields = {}, int scope_level=0, int line_num=1){
    this->name = name;
    Type::scope_level = scope_level;
    Type::line_num = line_num;
    this->fields = vector<Variable *>();

    for(auto it : fields){
        this->fields.push_back(it);
    }

}
int Structure::get_size(){
    int sum = 0;
    for (auto f: fields){
        sum += f->get_size();
    }
    return sum;
}

int Structure::get_offset(string name){
    int size = 0;
    for (auto it : fields){
        if (it->name.compare(name)==0){
            break;
        }
        size += it->get_size();
    }
    return size;
}

// init all relative variables
void init(){
    var_map = multimap<string, Variable*>();
    strc_map = multimap<string, Structure*>();
    global_scope_level = 0;
    EMPTY_TYPE = new Type();;
}

void print_logs(string msg, Node *node){
    /*cout << msg << endl;
    printf("    token:%s, value:%s, lineno:%d, child_num:%d\n", node->token.c_str(), node->value.c_str(), node->line_num, node->child_num);
    cout <<"    pattern:";
    for(auto it : node->child_list){
        cout << " " << it->token;
    }
    cout << endl;*/
}

Primitive *handle_Type(Node *node){
    print_logs("call handle_Type", node);
    return new Primitive(node->token);
}

/*
    Exp COMMA Args { $$ = new Node("Args", "", $1->line_num, 3, $1, $2, $3); }
    | Exp { $$ = new Node("Args", "", $1->line_num, 1, $1); }
    ;
*/
// return args list
vector<Type *> handle_Args(Node *node){
    print_logs("call handle_Args", node);
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
Type* handle_Exp(Node *node, bool single){
    print_logs("call handle_Exp", node);
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
    print_logs("call handle_VarDec", node);
    // use a stack to simulate recursion
    vector<Node *> stack = vector<Node *>();
    stack.push_back(node);
    string name;
    while(!stack.empty()){
        Node *tmp = stack.back();
        // handle ID
        if(tmp->child_list[0]->token == "ID"){
            stack.pop_back();
            name = tmp->child_list[0]->value;
            while(!stack.empty()){
                tmp = stack.back();
                type = new Array(type, atoi(node->child_list[2]->value.c_str()));
                stack.pop_back();
            }
        }else{
            stack.push_back(tmp->child_list[0]);
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
    print_logs("call handle_Dec", node);
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
    print_logs("call handle_DecList", node);
    vector<Variable *> l = vector<Variable *>();
    l.push_back(handle_Dec(node->child_list[0], type));
    Node *tmp = node;
    while(tmp->child_num > 1){
        tmp = tmp->child_list[2];
        l.push_back(handle_Dec(tmp->child_list[0], type));
    }
    return l;
}
/*
Def:
    Specifier DecList SEMI { $$ = new Node("Def", "", $1->line_num, 3, $1, $2, $3); }
    ;
*/
vector<Variable *> handle_Def(Node *node){
    print_logs("call handle_Def", node);
    return handle_DecList(node->child_list[1], handle_Specifier(node->child_list[0]));
}
/*
DefList:
    Def DefList { $$ = new Node("DefList", "", $1->line_num, 2, $1, $2); }
    | %empty { $$ = new Node("Empty", "", yylineno, 0); }
    ;
*/
vector<Variable *> handle_DefList(Node *node){
    print_logs("call handle_DefList", node);
    vector<Variable *> l = vector<Variable *>();
    Node *tmp = node;
    while(tmp->child_num){
        auto def = handle_Def(tmp->child_list[0]);
        l.insert(l.end(), def.begin(), def.end());
        tmp = tmp->child_list[1];
    }
    return l;
}
/*
StructSpecifier:
    STRUCT ID LC DefList RC { $$ = new Node("StructSpecifier", "", $1->line_num, 5, $1, $2, $3, $4, $5); }
    | STRUCT ID { $$ = new Node("StructSpecifier", "", $1->line_num, 2, $1, $2); }
    ;
*/
Type *handle_StructSpecifier(Node *node){
    print_logs("call handle_StructSpecifier", node);
    string name = node->value;
    if(node->child_num > 3){
        return new Structure(name, handle_DefList(node->child_list[3]));
    }
    return strc_map.find(name)->second;
}
/*
Specifier:
    TYPE { $$ = new Node("Specifier", "", $1->line_num, 1, $1); }
    | StructSpecifier { $$ = new Node("Specifier", "", $1->line_num, 1, $1); }
    ;
*/
Type *handle_Specifier(Node *node){
    print_logs("call Specifier", node);
    if(node->token == "TYPE"){
        return handle_Type(node->child_list[0]);
    }
    return handle_StructSpecifier(node->child_list[0]);
}

/*
ExtDecList:
    VarDec { $$ = new Node("ExtDecList", "", $1->line_num, 1, $1); }
    | VarDec COMMA ExtDecList { $$ = new Node("ExtDecList", "", $1->line_num, 3, $1, $2, $3); }
    ;
*/
void handle_ExtDecList(Node *node, Type *type){
    print_logs("call handle_ExtDecList", node);
    handle_VarDec(node->child_list[0], type);
    Node *tmp = node;
    while(tmp->child_num > 1){
        tmp = tmp->child_list[2];
        handle_VarDec(node->child_list[0], type);
    }
}

/*
ParamDec:
    Specifier VarDec { $$ = new Node("ParamDec", "", $1->line_num, 2, $1, $2); }
    ;
*/
Variable* handle_ParamDec(Node *node){
    print_logs("call handle_ParamDec", node);
    return handle_VarDec(node->child_list[1], handle_Specifier(node->child_list[0]));
}

/*
Stmt:
    Exp SEMI { $$ = new Node("Stmt", "", $1->line_num, 2, $1, $2); }
    | CompSt { $$ = new Node("Stmt", "", $1->line_num, 1, $1); }
    | RETURN Exp SEMI { $$ = new Node("Stmt", "", $1->line_num, 3, $1, $2, $3); }

    | IF LP Exp RP Stmt { $$ = new Node("Stmt", "", $1->line_num, 5, $1, $2, $3, $4, $5); }
    | IF LP Exp RP Stmt ELSE Stmt { $$ = new Node("Stmt", "", $1->line_num, 7, $1, $2, $3, $4, $5, $6, $7); }
    | WHILE LP Exp RP Stmt { $$ = new Node("Stmt", "", $1->line_num, 5, $1, $2, $3, $4, $5); }
    ;
*/
void handle_Stmt(Node *node, Type *type){
    print_logs("call handle_Stmt", node);
    if(node->child_list[0]->token == "CompSt"){
        handle_CompSt(node->child_list[0], type);
    }else if(node->child_list[0]->token == "Exp"){
        handle_Exp(node->child_list[0]);
    }else if(node->child_list[0]->token == "RETURN"){
        Type *return_type = handle_Exp(node->child_list[1], true);
    }else if(node->child_list[0]->token == "IF"){
        handle_Exp(node->child_list[2]);
        handle_Stmt(node->child_list[4], type);
        if(node->child_num > 5){
            handle_Stmt(node->child_list[6], type);
        }
    }else if(node->child_list[0]->token == "WHILE"){
        handle_Exp(node->child_list[2]);
        handle_Stmt(node->child_list[4], type);
    }
}
/*
StmtList:
    Stmt StmtList { $$ = new Node("StmtList", "", $1->line_num, 2, $1, $2); }
    | %empty { $$ = new Node("Empty", "", yylineno, 0); }
    ;
*/
void handle_StmtList(Node *node, Type *type){
    print_logs("call handle_StmtList", node);
    Node *tmp = node;
    while(tmp->child_num){
        handle_Stmt(tmp->child_list[0], type);
        tmp = tmp->child_list[1];
    }
}


/*
CompSt:
    LC DefList StmtList RC { $$ = new Node("CompSt", "", $1->line_num, 4, $1, $2, $3, $4); }
    ;
*/
void handle_CompSt(Node *node, Type *type){
    print_logs("call handle_CompSt", node);
    handle_DefList(node->child_list[1]);
    handle_StmtList(node->child_list[2], type);
}
/*
VarList:
    ParamDec COMMA VarList { $$ = new Node("VarList", "", $1->line_num, 3, $1, $2, $3); }
    | ParamDec { $$ = new Node("VarList", "", $1->line_num, 1, $1); }
    ;
*/
vector<Variable *> handle_VarList(Node *node){
    print_logs("call handle_VarList", node);
    vector<Variable *> l = vector<Variable *>();
    l.push_back(handle_ParamDec(node->child_list[0]));
    Node *tmp = node;
    while(tmp->child_num > 1){
        tmp = tmp->child_list[2];
        l.push_back(handle_ParamDec(tmp->child_list[0]));
    }
    return l;
}

/*
FunDec:
    ID LP VarList RP { $$ = new Node("FunDec", "", $1->line_num, 4, $1, $2, $3, $4); }
    | ID LP RP { $$ = new Node("FunDec", "", $1->line_num, 3, $1, $2, $3); }
    ;
*/
Variable* handle_FunDec(Node *node, Type *type){
    print_logs("call handle_FunDec", node);
    string name = node->value;
    vector<Variable *> args = vector<Variable *>();
    if(node->child_list[2]->token == "VarList"){
        args = handle_VarList(node->child_list[2]);
    }

    return new Variable(name, type, args, 1, global_scope_level - 1, node->line_num);
}
/*
ExtDef:
    Specifier ExtDecList SEMI { $$ = new Node("ExtDef", "", $1->line_num, 3, $1, $2, $3); }
    | Specifier SEMI { $$ = new Node("ExtDef", "", $1->line_num, 2, $1, $2); }
    | Specifier FunDec CompSt { $$ = new Node("ExtDef", "", $1->line_num, 3, $1, $2, $3); }
    ;
*/
void handle_ExtDef(Node *node){
    print_logs("call handle_ExtDef", node);
    // return type int, float, char or struct
    Type *type = handle_Specifier(node->child_list[0]);
    vector<Variable *> l = vector<Variable *>();
    if(node->child_list[1]->token == "ExtDecList"){
        handle_ExtDecList(node->child_list[1], type);
    }
    if(node->child_list[1]->token == "FunDec"){
        Variable *fundec = handle_FunDec(node->child_list[1], type);
        handle_CompSt(node->child_list[2], type);
    }
}
/*
ExtDefList:
    ExtDef ExtDefList { $$ = new Node("ExtDefList", "", $1->line_num, 2, $1, $2); }
    | %empty { $$ = new Node("ExtDefList", "", yylineno, 0); }
    ;
*/
void handle_ExtDefList(Node *node){
    print_logs("call handle_ExtDefList", node);
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
    print_logs("call handle_Programe", root);
    init();
    handle_ExtDefList(root->child_list[0]);
}