#include "tree.h"
#include <assert.h>
#include <map>
#include <stdarg.h>
#include <string.h>
using namespace std;
enum CheckInfo
{
    OK,
    VARIABLE_NOT_FOUND,
    FUNCTION_NOT_FOUND,
    VARIABLE_REDEFINED,
    FUNCTION_REDEFINED,
    UNMATCHING_TYPE_ON_ASSIGN,
    RVALUE_ON_THE_LEFT,
    UNMATCHING_OPERANDS, 
    MISMATCH_RETURN_TYPE, 
    MISMATCH_FUNCTION_ARGUMENTS, 
    APPLYING_INDEXING_FOR_NON_ARRAY, 

    APPLY_FUNCTION_ON_VARIABLE,
    APPLY_INDEXING_WITH_NON_INT, 
    
    
    ACCESSING_MEMBER_OF_NON_STRUCTURE_VAR, 
    STRUCTURE_NOT_FOUND,
    
    STRUCTURE_REDEFINED,
};
enum SPL_TYPE{
    C_EMPTY,
    C_PRIMITIVE, 
    C_ARRAY, 
    C_STRUCTURE
};
class Type;
class CVariable;
class CStructure;
class CArray;


bool check_type(Type* left, Type* right);



extern Type* EMPTY_TYPE;
extern CVariable* EMPTY_VARIABLE;

extern multimap<string, CVariable*> var_map;
extern multimap<string, CStructure*> structure_map;

extern int global_scope_level;



class Type{
public:
    int scope_level;
    int lineno;
    SPL_TYPE type_id;
    Type(int scope_level = 0, int lineno = 0, SPL_TYPE type_id = C_EMPTY) 
        : scope_level(scope_level), lineno(lineno), type_id(type_id)
        {}
    virtual bool operator==(const Type& other) const{
        return this->type_id == other.type_id;
    }
    virtual bool operator!=(const Type& other) const{
        return this->type_id != other.type_id;
    }

};

// primitive type of spl
class CPrimitive : public Type{
public:
    Token basic_type;
    CPrimitive(Token basic_type, int scope_level = 0, int lineno = 0)
        : basic_type(basic_type), Type::Type(scope_level, lineno, C_PRIMITIVE) 
        {
            assert(basic_type == T_INT || basic_type == T_FLOAT || basic_type == T_CHAR);
        }
    bool operator==(const Type& other) const {
        try{
            return this->type_id == other.type_id && 
                this->basic_type == dynamic_cast<const CPrimitive&>(other).basic_type;
        }
        catch(...){
            printf("Something interesting happened\n");
            return false;
        }
        
    }
};

class CArray : public Type{
public:
    Type* base_type;
    int array_size;
    CArray(Type* base_type, int array_size, int scope_level = 0, int lineno = 0)
        : base_type(base_type), array_size(array_size), 
        Type::Type(scope_level, lineno, C_ARRAY){}

    bool operator==(const Type& other) const{
        
        auto other_type = dynamic_cast<const CArray&>(other);
        return this->type_id == other.type_id
            && array_size == other_type.array_size
            && *base_type == *(other_type.base_type);
    }

};
class CVariable{
public:
    string name;
    Type* type;
    vector<CVariable*> args;
    int scope_level;
    int lineno;
    bool is_function;
    CVariable(string name, Type* type, vector<CVariable*> args = {}, 
        int scope_level = 0, int lineno = 0, bool is_function = false) 
            : name(name), type(type), args(args), 
            scope_level(scope_level), lineno(lineno), is_function(is_function)
            {

            }
    bool CheckArgs(vector<Type*> type_args){
        if(args.size() != type_args.size()) return false;
        
        for(int i = 0; i < args.size(); i++){
            
            if(!check_type(args[i]->type, type_args[i])){
                return false;
            }
        }
        return true;
    }
    bool operator==(const CVariable& other) const {
        
        if(name != other.name || args.size() != other.args.size()) return false;
        int args_num = args.size();
        for(int i = 0; i < args_num; i++){
            if(*(args[i]->type) != *(other.args[i]->type)){
                return false;
            }
        }
        return true;
    }
    bool operator!=(const CVariable& other) const {
        return !(*this == other);
    }  
};

class CStructure : public Type{
public:
    string name;
    vector<CVariable*> fields;
    string to_string(){
        return name;
    }
    CStructure(string name, vector<CVariable*> fields = {}, 
    int scope_level = 0, int lineno = 0) 
    : name(name), fields(fields), 
    Type::Type(scope_level, lineno, C_STRUCTURE)
    {}

    Type* GetMemberType(string a){
        
        for(auto it : fields){
            if(it->name == a){
                
                return it->type;
            }
        }
        
        return NULL;
    }
    bool operator==(const Type& other) const{
        return this->type_id == other.type_id 
        && this->name == dynamic_cast<const CStructure&>(other).name;
    }
};


void ProcessProgram(Node* current_node);
void ProcessExtDefList(Node* current_node);
void ProcessExtDef(Node* current_node);
Type* ProcessSpecifier(Node* current_node);
CPrimitive* ProcessType(Node* current_node);
Type* ProcessStructSpecifier(Node* current_node);

vector<CVariable*> ProcessDefList(Node* current_node);
vector<CVariable*> ProcessDef(Node* current_node);
vector<CVariable*> ProcessDecList(Node* current_node, Type* type);

CVariable* ProcessDec(Node* current_node, Type* type);
CVariable* ProcessVarDec(Node* current_node, Type* type);
string ProcessID(Node* current_node);
void ProcessExtDecList(Node* current_node, Type* type);
CVariable* ProcessFunDec(Node* current_node, Type* type);
vector<CVariable*> ProcessVarList(Node* current_node);
CVariable* ProcessParamDec(Node* current_node);
Type* ProcessExp(Node* current_node, bool single = false);
vector<Type*> ProcessArgs(Node* current_node);
void ProcessCompSt(Node* current_node, Type* type);
void ProcessStmtList(Node* current_node, Type* type);
void ProcessStmt(Node* current_node, Type* type);

void UpdateVariable(vector<CVariable*> new_variables);
void UpdateStructure(vector<CStructure*> new_structures);

pair<CVariable*, int> GetVariable(string name, vector<Type*> args, bool is_function);
pair<Type*, int> GetStructure(string name);
pair<CVariable*, int> GetLocalVariable(string name, vector<Type*> args, bool is_function);
pair<Type*, int> GetLocalStructure(string name);

void PushScope();
void PopScope();
bool is_digit(string s);
bool SemanticErrorReport(int state, int lineinfo);