#include "semantic.h"

multimap<string, CVariable*> var_map;
multimap<string, CStructure*> structure_map;

int global_scope_level;
extern FILE* fout;

Type* EMPTY_TYPE = new Type();
CVariable* EMPTY_VARIABLE = new CVariable("", EMPTY_TYPE);

void initialize_all(){
    var_map.clear();
    structure_map.clear();
    global_scope_level = 0;
}

bool check_type(Type* left, Type* right){
    
    if(*left == *EMPTY_TYPE || *right == *EMPTY_TYPE){
        return false;
    }
    return *left == *right;
}


void ProcessProgram(Node* current_node){
    /*
    ExtDefList
    */
    initialize_all();
    if(current_node == NULL){
        fprintf(fout,"This is not a valid program\n");
        return;
    }
    ProcessExtDefList(current_node->children[0]);
}
void ProcessExtDefList(Node* current_node){
    /*
    ExtDefList:
        ExtDef ExtDefList
        | empty
    */
   while(current_node->child_num > 0){
       ProcessExtDef(current_node->children[0]);
       current_node = current_node->children[1];
   }
}
void ProcessExtDef(Node* current_node){
    /*
    ExtDef:
        Specifier ExtDecList SEMI {$$ = new Node(ExtDef, 1, "ExtDef", @1.first_line, 3, $1, $2, $3);}
        | Specifier SEMI {$$ = new Node(ExtDef, 1, "ExtDef", @1.first_line, 2, $1, $2);}
        | Specifier FunDec CompSt { $$ = new Node(ExtDef, 1, "ExtDef", @1.first_line, 3, $1, $2, $3);}
        ;
    */
    // 所有的Extdef都有一个specifier, 所以先把specifier的type拿到
   Type* type = ProcessSpecifier(current_node->children[0]);
   // 然后这个ExtDef有两种可能性, 一种是声明一系列变量, 
    // 这种情况下需要挨个给这些变量注册到表里面
   vector<CVariable*> args;
   if(current_node->children[1]->type == ExtDecList){
       
       ProcessExtDecList(current_node->children[1], type);
   }
   else if(current_node->children[1]->type == FunDec){
       PushScope();
       CVariable* function_declare = ProcessFunDec(current_node->children[1], type);
       ProcessCompSt(current_node->children[2], type);

       PopScope();
       
       UpdateVariable({function_declare});
   }
}
Type* ProcessSpecifier(Node* current_node){
    /*
Specifier:
        TYPE {$$ = new Node(Specifier, 1, "Specifier", @1.first_line, 1, $1); }
        | StructSpecifier {$$ = new Node(Specifier, 1, "Specifier", @1.first_line, 1, $1);}
        ;
    */
    if(current_node->children[0]->type == T_TYPE){
        return ProcessType(current_node->children[0]);
    }
    else if(current_node->children[0]->type == StructSpecifier){
        return ProcessStructSpecifier(current_node->children[0]);
    }
    else{
        assert(false);
    }


}
CPrimitive* ProcessType(Node* current_node){
    // primitive type: int, float, char
    if(current_node->value == "int"){
        return new CPrimitive(T_INT);
    }
    if(current_node->value == "char"){
        return new CPrimitive(T_CHAR);
    }
    if(current_node->value == "float"){
        return new CPrimitive(T_FLOAT);
    }
    assert(false);
    return nullptr;
}
Type* ProcessStructSpecifier(Node* current_node){
    /*
    StructSpecifier:
        STRUCT ID LC DefList RC {$$ = new Node(StructSpecifier, 1, "StructSpecifier", @1.first_line, 5, $1, $2, $3, $4, $5);}
        | STRUCT ID {$$ = new Node(StructSpecifier, 1, "StructSpecifier", @1.first_line, 2, $1, $2);}
        ;
    */
    string name = ProcessID(current_node->children[1]);
    
    if(current_node->child_num > 2){
        PushScope();
        //这里push之后structure本身不应该收到影响, 所以要-1修正
        CStructure* new_structure = new CStructure(name, ProcessDefList(current_node->children[3]), 
                                                                                                 global_scope_level-1, current_node->lineno);
        PopScope();
        UpdateStructure({new_structure});
        return new_structure;
    }
    else{
        auto res = GetStructure(name);
        SemanticErrorReport(res.second, current_node->children[0]->lineno);
        return res.first;
    }
    
}

vector<CVariable*> ProcessDefList(Node* current_node){
    /*
DefList:
        Def DefList { $$ = new Node(DefList, 1, "DefList", @1.first_line, 2, $1, $2);}
        |  {$$ = new Node(DefList, 1, "DefList", yylineno, 0); }
        ;
    */

   vector<CVariable*> vars;
   while(current_node->child_num > 0){
       auto def_list = ProcessDef(current_node->children[0]);
       vars.insert(vars.end(), def_list.begin(), def_list.end());
       current_node = current_node->children[1];
   }
   return vars;

}
vector<CVariable*> ProcessDef(Node* current_node){
    /*
    Def:
        Specifier DecList SEMI {$$ = new Node(Def, 1, "Def", @1.first_line, 3, $1, $2, $3); }
        ;
    */
   
   Type* type = ProcessSpecifier(current_node->children[0]);
    auto def_list = ProcessDecList(current_node->children[1], type);
    return def_list;
}
vector<CVariable*> ProcessDecList(Node* current_node, Type* type){
    /*
DecList:
        Dec { $$ = new Node(DecList, 1, "DecList", @1.first_line, 1, $1);}
        | Dec COMMA DecList {$$ = new Node(DecList, 1, "DecList", @1.first_line, 3, $1, $2, $3); }
        ;
    */
    vector<CVariable*> vars;
    vars.push_back(ProcessDec(current_node->children[0], type));
    while(current_node->child_num > 1){
        current_node = current_node->children[2];
        vars.push_back(ProcessDec(current_node->children[0], type));
    }
    UpdateVariable(vars);

    return vars;
}

CVariable* ProcessDec(Node* current_node, Type* type){
    /*
   Dec:
        VarDec {$$ = new Node(Dec, 1, "Dec", @1.first_line, 1, $1); }
        | VarDec ASSIGN Exp {$$ = new Node(Dec, 1, "Dec", @1.first_line, 3, $1, $2, $3); }
    ;
    */
   
   CVariable* dec = ProcessVarDec(current_node->children[0], type);
   if(current_node->child_num > 1){
       Type* exp = ProcessExp(current_node->children[2]);
       if(!check_type(type, exp)){
           fprintf(fout,"Error type 5 at Line %d: unmatching type\n", current_node->children[1]->lineno);
           
       }
   }
   return dec;
}
CVariable* ProcessVarDec(Node* current_node, Type* type){
    /*
VarDec:
        VarDec LB INT RB {$$ = new Node(VarDec, 1, "VarDec", @1.first_line, 4, $1, $2, $3, $4);}
        | ID {$$ = new Node(VarDec, 1, "VarDec", @1.first_line, 1, $1);}
      
    */
   
    vector<Node*> stack;
    stack.push_back(current_node);
    string name;
    while(!stack.empty()){
        Node* temp = stack.back();
        // 疯狂递归找到最左边的ID, 解决类似arr[1][2][3]这种形式的问题
        // 然后递归把这个array建起来
        if(temp->children[0]->type == T_ID){
            name = temp->children[0]->value;
            stack.pop_back();
            while(!stack.empty()){
                temp = stack.back();
                stack.pop_back();

                string value = temp->children[2]->value;
                if(is_digit(value)){
                    type = new CArray(type, atoi(value.c_str()));
                }
                else{
                    fprintf(fout,"Error type 12 at Line %d: array index is not int\n", temp->lineno);
                    return EMPTY_VARIABLE;
                }
            }
        }
        else if(temp->children[0]->type == VarDec){
            stack.push_back(temp->children[0]);
        }
        else{
            fprintf(fout,"False\n");
            assert(false);
        }
    }
    

    return new CVariable(name, type, {}, global_scope_level, current_node->lineno, false);

}
string ProcessID(Node* current_node){
    return current_node->value;
}
void ProcessExtDecList(Node* current_node, Type* type){
    /*
    ExtDecList:
        VarDec { $$ = new Node(ExtDecList, 1, "ExtDecList", @1.first_line, 1, $1);}
        | VarDec COMMA ExtDecList {$$ = new Node(ExtDecList, 1, "ExtDecList", @1.first_line, 3, $1, $2, $3);}
        ;
    */
}
CVariable* ProcessFunDec(Node* current_node, Type* type){
    /*
FunDec:
        ID LP VarList RP { $$ = new Node(FunDec, 1, "FunDec", @1.first_line, 4, $1, $2, $3, $4);}
        | ID LP RP {$$ = new Node(FunDec, 1, "FunDec", @1.first_line, 3, $1, $2, $3); }
        ;
    */
   string name = ProcessID(current_node->children[0]);
   vector<CVariable*> args;
   if(current_node->child_num > 3){
       args = ProcessVarList(current_node->children[2]);
   }
   UpdateVariable(args);
   return new CVariable(name, type, args, global_scope_level-1, current_node->lineno, 1);

}
vector<CVariable*> ProcessVarList(Node* current_node){
    vector<CVariable*> vars;
    vars.push_back(ProcessParamDec(current_node->children[0]));
    while(current_node->child_num > 1){
        current_node = current_node->children[2];
         vars.push_back(ProcessParamDec(current_node->children[0]));
    }
    return vars;

}
CVariable* ProcessParamDec(Node* current_node){
    Type* type = ProcessSpecifier(current_node->children[0]);
    CVariable* arg = ProcessVarDec(current_node->children[1], type);
    return arg;
}
Type* ProcessExp(Node* current_node, bool single){
    /*
    Exp ASSIGN Exp { $$ = new Node(Exp, 1, "Exp", @1.first_line, 3, $1, $2, $3); }
        | Exp AND Exp { $$ = new Node(Exp, 1, "Exp", @1.first_line, 3, $1, $2, $3); }
        | Exp OR Exp { $$ = new Node(Exp, 1, "Exp", @1.first_line, 3, $1, $2, $3); }
        | Exp LT Exp { $$ = new Node(Exp, 1, "Exp", @1.first_line, 3, $1, $2, $3); }
        | Exp LE Exp { $$ = new Node(Exp, 1, "Exp", @1.first_line, 3, $1, $2, $3); }
        | Exp GT Exp { $$ = new Node(Exp, 1, "Exp", @1.first_line, 3, $1, $2, $3); }
        | Exp GE Exp { $$ = new Node(Exp, 1, "Exp", @1.first_line, 3, $1, $2, $3); }
        | Exp NE Exp { $$ = new Node(Exp, 1, "Exp", @1.first_line, 3, $1, $2, $3); }
        | Exp EQ Exp { $$ = new Node(Exp, 1, "Exp", @1.first_line, 3, $1, $2, $3); }
        | Exp ADD Exp { $$ = new Node(Exp, 1, "Exp", @1.first_line, 3, $1, $2, $3); }
        | Exp SUB Exp { $$ = new Node(Exp, 1, "Exp", @1.first_line, 3, $1, $2, $3); }
        | Exp MUL Exp { $$ = new Node(Exp, 1, "Exp", @1.first_line, 3, $1, $2, $3); }
        | Exp DIV Exp { $$ = new Node(Exp, 1, "Exp", @1.first_line, 3, $1, $2, $3); }
        
        | LP Exp RP {$$ = $2; }
        | SUB Exp {$$ = new Node(Exp, 1, "Exp", @1.first_line, 2, $1, $2); }
        | NOT Exp {$$ = new Node(Exp, 1, "Exp", @1.first_line, 2, $1, $2);}
        
        | ID LP Args RP {$$ = new Node(Exp, 1, "Exp", @1.first_line, 4, $1, $2, $3, $4);}
        | ID LP RP {$$ = new Node(Exp, 1, "Exp", @1.first_line, 3, $1, $2, $3);}
        
        | Exp LB Exp RB {$$ = new Node(Exp, 1, "Exp", @1.first_line, 4, $1, $2, $3, $4);}
        | Exp DOT ID {$$ = new Node(Exp, 1, "Exp", @1.first_line, 3, $1, $2, $3);}

        | ID { $$ = new Node(Exp, 1, "Exp", @1.first_line, 1, $1); }
        | INT {$$ = new Node(Exp, 1, "Exp", @1.first_line, 1, $1); }
        | FLOAT { $$ = new Node(Exp, 1, "Exp", @1.first_line, 1, $1); }
        | CHAR { $$ = new Node(Exp, 1, "Exp", @1.first_line, 1, $1); }
        ;
    */
   
   if(current_node->child_num == 1 && current_node->children[0]->type != T_ID){
       if(single){
           fprintf(fout,"Error type 6 at Line %d: rvalue on the left side\n", current_node->children[0]->lineno);
           //PrintError("Error type 6 at Line %d: rvalue on the left side", current_node->children[0]->lineno);
           return EMPTY_TYPE;
       }

       return new CPrimitive(Token(current_node->children[0]->type));
   }
   auto first_type = current_node->children[0]->type;
    if(first_type == T_ID){
        string name = ProcessID(current_node->children[0]);
        
        vector<Type*> args;
        bool is_function_call = false;
        if(current_node->child_num > 1){
            if(current_node->child_num > 3){
                args = ProcessArgs(current_node->children[2]);
            }
            is_function_call = true;
        }
        
        auto result = GetVariable(name, args, is_function_call);
        CVariable* var = result.first;
        SemanticErrorReport(result.second, current_node->children[0]->lineno);
        return var->type;
    }
    if(first_type == T_NOT || first_type == T_SUB){
        return ProcessExp(current_node->children[1]);
    }
    
    if(current_node->children[1]->type == T_DOT){
        // Exp DOT ID {$$ = new Node(Exp, 1, "Exp", @1.first_line, 3, $1, $2, $3);}
        
        
        Type* type = ProcessExp(current_node->children[0]);
        
        if(type->type_id != C_STRUCTURE){
            
            fprintf(fout,"Error type 13 at Line %d: accessing member of non-structure variable\n", current_node->children[0]->lineno);
            return EMPTY_TYPE;
        }
        
        CStructure* structure_type = dynamic_cast<CStructure*>(type);
        type = structure_type->GetMemberType(ProcessID(current_node->children[2]));
        if(type == NULL){
            fprintf(fout,"Error type 14 at Line %d: accessing an undefined structure member\n", current_node->children[2]->lineno);
            return EMPTY_TYPE;
        }
        
        return type;
    }
    
    if(current_node->children[1]->type == T_LB){
        
        // Exp LB Exp RB {$$ = new Node(Exp, 1, "Exp", @1.first_line, 4, $1, $2, $3, $4);}
        
        Type* type = ProcessExp(current_node->children[0]);
        Type* index = ProcessExp(current_node->children[2]);
        
        if(index->type_id != C_PRIMITIVE 
            || dynamic_cast<CPrimitive*>(index)->basic_type != T_INT){
                fprintf(fout,"Error type 12 at Line %d: array indexing with non-integer type expression\n", current_node->children[2]->lineno);
                return EMPTY_TYPE;
        }
        if(type->type_id != C_ARRAY){
            fprintf(fout,"Error type 10 at Line %d: applying indexing operator ([...]) on non-array type variables\n", current_node->children[0]->lineno);
            return EMPTY_TYPE;
        }
        
        return dynamic_cast<CArray*>(type)->base_type;

    }
    if(current_node->child_num == 3){
        Type* left = ProcessExp(current_node->children[0], current_node->children[1]->type == T_ASSIGN);
        Type* right = ProcessExp(current_node->children[2]);
        
        if(!check_type(left, right)){
            string msg = current_node->children[1]->type == T_ASSIGN ? 
            "Error type 5 at Line %d: unmatching types on both sides of assignment operator (=)\n" : 
            "Error type 7 at Line %d: unmatching operands, such as adding an integer to a structure variable\n";
            
            fprintf(fout,msg.c_str(), current_node->children[1]->lineno );
            return EMPTY_TYPE;
        }
        return left;
    }
}
vector<Type*> ProcessArgs(Node* current_node){
    /*
Args: Exp COMMA Args {$$ = new Node(Args, 1, "Args", @1.first_line, 3, $1, $2, $3);}
        | Exp { $$ = new Node(Args, 1, "Args", @1.first_line, 1, $1);}
    */
   vector<Type*> args;
   args.push_back(ProcessExp(current_node->children[0]));
   while(current_node->child_num > 1){
       current_node = current_node->children[2];
       args.push_back(ProcessExp(current_node->children[0]));
   }
   return args;
}
void ProcessCompSt(Node* current_node, Type* type){
    /*
CompSt:
        LC DefList StmtList RC { $$ = new Node(CompSt, 1, "CompSt", @1.first_line, 4, $1, $2, $3, $4);}
        ;
    */
   ProcessDefList(current_node->children[1]);
   
   ProcessStmtList(current_node->children[2], type);

}
void ProcessStmtList(Node* current_node, Type* type){
    /*
StmtList:
        Stmt StmtList {$$ = new Node(StmtList, 1, "StmtList", @1.first_line, 2, $1, $2); }
        | {$$ = new Node(StmtList, 1, "StmtList", yylineno, 0);}
        ;
    */
   while(current_node->child_num > 0){
       
        ProcessStmt(current_node->children[0], type);
        
        current_node = current_node->children[1];
   }
}
void ProcessStmt(Node* current_node, Type* type){
    /*
Stmt: 
        Exp SEMI {$$ = new Node(Stmt, 1, "Stmt", @1.first_line, 2, $1, $2);}
        | CompSt { $$ = new Node(Stmt, 1, "Stmt", @1.first_line, 1, $1);}
        | RETURN Exp SEMI { $$ = new Node(Stmt, 1, "Stmt", @1.first_line, 3, $1, $2, $3);}
        
        | IF LP Condition RP Stmt { $$ = new Node(Stmt, 1, "Stmt", @1.first_line, 5, $1, $2, $3, $4, $5);}
        | IF LP Condition RP Stmt ELSE Stmt {$$ = new Node(Stmt, 1, "Stmt", @1.first_line, 7, $1, $2, $3, $4, $5, $6, $7);}
        
        
        | WHILE LP Exp RP Stmt { $$ = new Node(Stmt, 1, "Stmt", @1.first_line, 5, $1, $2, $3, $4, $5);}
        | FOR LP ForDef SEMI ForCon SEMI ForInc RP Stmt { $$ = new Node(Stmt, 1, "Stmt", @1.first_line, 0);}
        | BREAK SEMI { $$ = new Node(Stmt, 1, "Stmt", @1.first_line, 0);}
        | CONTINUE SEMI { $$ = new Node(Stmt, 1, "Stmt", @1.first_line, 0);}
        ;
    */

   if(current_node->children[0]->type == Exp){
       ProcessExp(current_node->children[0]);
   }
   if(current_node->children[0]->type == CompSt){
       PushScope();
       ProcessCompSt(current_node->children[0], type);
       PopScope();
   }
   if(current_node->children[0]->type == T_RETURN){
       Type* return_type = ProcessExp(current_node->children[1]);
       if(!check_type(return_type, type)){
           
           fprintf(fout,"Error type 8 at Line %d: the function’s return value type mismatches the declared type\n", current_node->children[0]->lineno);
       }
   }
   if(current_node->children[0]->type == T_IF){
       //| IF LP Condition RP Stmt { $$ = new Node(Stmt, 1, "Stmt", @1.first_line, 5, $1, $2, $3, $4, $5);}
       // | IF LP Condition RP Stmt ELSE Stmt {$$ = new Node(Stmt, 1, "Stmt", @1.first_line, 7, $1, $2, $3, $4, $5, $6, $7);}
        ProcessExp(current_node->children[2]);
        ProcessStmt(current_node->children[4], type);
        if(current_node->child_num > 5){
            ProcessStmt(current_node->children[6], type);
        }
   }
   if(current_node->children[0]->type == T_WHILE){
       //WHILE LP Exp RP Stmt { $$ = new Node(Stmt, 1, "Stmt", @1.first_line, 5, $1, $2, $3, $4, $5);}
       PushScope();
        ProcessExp(current_node->children[2]);
        ProcessStmt(current_node->children[4], type);
       PopScope();
   }

}


void UpdateVariable(vector<CVariable*> new_variables){
    
    
    for(auto var : new_variables){
        vector<Type*> args;
        for(auto it : var->args){
            args.push_back(it->type);
        }
        auto res = GetLocalVariable(var->name, args, var->is_function);
        if(SemanticErrorReport(res.second, var->lineno)){
            var_map.insert(make_pair(var->name, var));
        }
    }

}
void UpdateStructure(vector<CStructure*> new_structures){
    for(auto type : new_structures){
        auto res = GetLocalStructure(type->name);
        if(SemanticErrorReport(res.second, type->lineno))
            structure_map.insert(make_pair(type->name, type));
    }
}


pair<Type*, int> GetStructure(string name){
    auto it = structure_map.find(name);
    int cnt = structure_map.count(name);
    int cur_scope_level = -1;
    Type* res = EMPTY_TYPE;
    int flag = STRUCTURE_NOT_FOUND;
    while(cnt--){
        if(it->second->scope_level > cur_scope_level){
            res = it->second;
            cur_scope_level = it->second->scope_level;
            flag = OK;
        }
        it++;
    }
    return make_pair(res, flag);
}

pair<Type*, int> GetLocalStructure(string name){
    auto it = structure_map.find(name);
    int cnt = structure_map.count(name);
    Type* res = EMPTY_TYPE;
    int flag = OK;
    while(cnt--){
        if(it->second->scope_level == global_scope_level){
            res = it->second;
            flag = STRUCTURE_REDEFINED;
            break;
        }
        it++;
    }
    return make_pair(res, flag);
}
pair<CVariable*, int> GetVariable(string name, vector<Type*> args, bool is_function){
    // 调用函数或变量时使用
    int flag = is_function ? FUNCTION_NOT_FOUND : VARIABLE_NOT_FOUND;
    CVariable* result = EMPTY_VARIABLE;
    auto it = var_map.find(name);
    
    if(it == var_map.end()){  return make_pair(result, flag);}
    
    int cnt = var_map.count(name);
    int current_scope_level = -1;
    
    
    while(cnt--){
        if(!it->second->is_function && is_function){
            flag = APPLY_FUNCTION_ON_VARIABLE;
        }
        else if(it->second->CheckArgs(args)){
            if(it->second->scope_level > current_scope_level){
                flag = OK;
                result = it->second;
                current_scope_level = it->second->scope_level;
            }
            else{
            }
        }
        else{
            if(flag != OK){
                flag = MISMATCH_FUNCTION_ARGUMENTS;
                result = it->second;
            }
        }
        it++;
    }
    return make_pair(result, flag);
}

pair<CVariable*, int> GetLocalVariable(string name, vector<Type*> args, bool is_function){
    // 定义变量或函数时使用
    
    auto it = var_map.find(name);
    int cnt = var_map.count(name);
    CVariable* res = EMPTY_VARIABLE;
    int flag = OK;
    
    
    while(cnt-- > 0){
        if(it->second->scope_level == global_scope_level && it->second->CheckArgs(args)){
            res = it->second;
            flag = it->second->is_function ? FUNCTION_REDEFINED : VARIABLE_REDEFINED;
            break;
        }
        it++;
    }
    return make_pair(res, flag);
}


void PushScope(){
    global_scope_level++;
}
void PopScope(){
    for(auto it = structure_map.begin(); it != structure_map.end(); it++){
        if(it->second->scope_level == global_scope_level){
            structure_map.erase(it);
        }
    }
    for(auto it = var_map.begin(); it != var_map.end(); it++){
        if(it->second->scope_level == global_scope_level){
            var_map.erase(it);
        }
    }
    global_scope_level--;

}
bool is_digit(string s){
    return( strspn( s.c_str(), "0123456789" ) == s.size() );
}

bool SemanticErrorReport(int state, int lineinfo){
    if(state == 1){
        fprintf(fout,"Error type %d at Line %d: variable is used without definition\n", state, lineinfo);
    }
    else if(state == 2){
        fprintf(fout,"Error type %d at Line %d: function is invoked without definition\n" , state, lineinfo);
    }
    else if(state == 3){
        fprintf(fout,"Error type %d at Line %d: variable is redefined in the same scope\n" , state, lineinfo);
    }
    else if(state == 4){
        fprintf(fout,"Error type %d at Line %d: function is redefined\n" , state, lineinfo);
    }
    else if(state == 5){
        fprintf(fout,"Error type %d at Line %d: unmatching types on both sides of assignment operator (=) \n" , state, lineinfo);
    }
    else if(state == 6){
        fprintf(fout,"Error type %d at Line %d: rvalue on the left side of assignment operator\n" , state, lineinfo);
    }
    else if(state == 7){
        fprintf(fout,"Error type %d at Line %d: unmatching operands\n" , state, lineinfo);
    }
    else if(state == 8){
        fprintf(fout,"Error type %d at Line %d: the function’s return value type mismatches the declared type\n" , state, lineinfo);
    }
    else if(state == 9){
        fprintf(fout,"Error type %d at Line %d: the function’s arguments mismatch the declared parameters\n" , state, lineinfo);
    }
    else if(state == 10){
        fprintf(fout,"Error type %d at Line %d: applying indexing operator ([...]) on non-array type variables\n" , state, lineinfo);
    }
    else if(state == 11){
        fprintf(fout,"Error type %d at Line %d: applying function invocation operator (foo(...)) on non-function names\n" , state, lineinfo);
    }
    else if(state == 12){
        fprintf(fout,"Error type %d at Line %d: array indexing with non-integer type expression\n" , state, lineinfo);
    }
    else if(state == 13){
        fprintf(fout,"Error type %d at Line %d: accessing member of non-structure variable\n" , state, lineinfo);
    }
    else if(state == 14){
        fprintf(fout,"Error type %d at Line %d: accessing an undefined structure member\n" , state, lineinfo);
    }
    else if(state == 15){
        fprintf(fout,"Error type %d at Line %d: redefine the same structure type\n" , state, lineinfo);
    }
    if(state <= 15){
        return true;
    }
    else{
        return true;
    }
    
}