#include "tree.hpp"

using namespace std;
int type_size;

class Type{
    public:
        int scope_level;
        int line_num;
        Type(int, int);
        virtual bool operator == (const Type &other) const {
            return (typeid(*this)==typeid(other));
        }
};
Type::Type(int scope_level=0, int line_num=1){
    this->scope_level = scope_level;
    this->line_num = line_num;
}

class Primitive: public Type{
    public:
        string token;
        Primitive(string, int, int);
};
Primitive::Primitive(string token, int scope_level=0, int line_num=1){
    this->token = token;
    Type::scope_level = scope_level;
    Type::line_num = line_num;
}

class Array: public Type{
    public:
        Type *type;
        int size;
        Array(Type *, int, int, int);
};
Array::Array(Type *type, int size, int scope_level=0, int line_num=1){
    this->type = type;
    this->size = size;
    Type::scope_level = scope_level;
    Type::line_num = line_num;
}

class Variable{
    public:
        Type *type;
        string name;
        int scope_level;
        int line_num;
        bool is_func;
        vector<Variable *> args;
        Variable(string, Type *, vector<Variable *>, int, int, int);
        int get_size();
};
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

class Structure: public Type{
    public:
        string name;
        vector<Variable *> fields;
        Structure(string, vector<Variable *>, int, int);
        int get_offset(string);
        int get_size();
};
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

void init();
Primitive *handle_Type(Node *node);
vector<Type *> handle_Args(Node *node);
Type* handle_Exp(Node *node, bool single=false);
Variable* handle_VarDec(Node *node, Type *type);
Variable* handle_Dec(Node *node, Type *type);
vector<Variable *> handle_DecList(Node *node, Type *type);
vector<Variable *> handle_Def(Node *node);
vector<Variable *> handle_DefList(Node *node);
Type *handle_StructSpecifier(Node *node);
Type *handle_Specifier(Node *node);
void handle_ExtDecList(Node *node, Type *type);
Variable* handle_ParamDec(Node *node);
void handle_Stmt(Node *node, Type *type);
void handle_StmtList(Node *node, Type *type);
void handle_CompSt(Node *node, Type *type);
vector<Variable *> handle_VarList(Node *node);
Variable* handle_FunDec(Node *node, Type *type);
void handle_ExtDef(Node *node);
void handle_ExtDefList(Node *node);
void handle_Program(Node *root);

multimap<string, Variable *> var_map;
multimap<string, Structure *> strc_map;