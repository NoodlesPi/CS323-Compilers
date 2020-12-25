#include <bits/stdc++.h>
#include "tree.hpp"

using namespace std;

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

class Structure: public Type{
    public:
        string name;
        vector<Variable *> fields;
        Structure(string, vector<Variable *>, int, int);
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