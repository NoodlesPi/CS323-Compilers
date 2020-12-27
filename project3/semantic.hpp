#include "tree.hpp"

using namespace std;
extern const int type_size;

class Type{
    public:
        int scope_level;
        int line_num;
        Type(int, int);
        virtual bool operator == (const Type &other) const {
            return (typeid(*this)==typeid(other));
        }
        virtual int get_size(){
            return type_size;
        }
};

class Primitive: public Type{
    public:
        string token;
        Primitive(string, int = 0, int = 1);
};

class Array: public Type{
    public:
        Type *type;
        int size;
        Array(Type *, int, int, int);
};

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


class Structure: public Type{
    public:
        string name;
        vector<Variable *> fields;
        Structure(string, vector<Variable *>, int, int);
        int get_offset(string);
        int get_size();
        bool operator==(const Type &other) const{
        return (typeid(*this)==typeid(other)) && (name.compare(dynamic_cast<const Structure &>(other).name)==0);
        }
};

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

extern multimap<string, Variable *> var_map;
extern multimap<string, Structure *> strc_map;