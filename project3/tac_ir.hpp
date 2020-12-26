#include <bits/stdc++.h>
#include "semantic.hpp"

using namespace std;

const int len = 128;
const int type_size = 4;
vector<int> con_addr, br_addr;

class TAC{
    public:
        string name;
        int addr;
        bool is_swap;
        Type *type;
        TAC();
        TAC(bool);
        virtual string to_instruction(){
            return "";
        }
        string addr_to_string(int);
};
TAC::TAC(){
    bool is_swap = false;
}
TAC::TAC(bool is_swap){
    this->is_swap = is_swap;
}
string TAC::addr_to_string(int addr){
    char str[len];
    if(addr > 0){
        sprintf(str, "t%d", addr);
    }else{
        sprintf(str, "#%d", -addr);
    }
    return str;
}

class FuncCode: public TAC{
    public:
        string func_name;
        string to_instruction();
        FuncCode(string, int);
};
FuncCode::FuncCode(string func_name, int addr){
    this->func_name = func_name;
    TAC::addr = addr;
}
string FuncCode::to_instruction(){
    return "FUNCTION " + func_name + " :";
}

class ParamCode: public TAC{
    public:
        vector<int> suffix;
        vector<int> sizes;
        ParamCode(int, Type *, vector<int>);
};
ParamCode::ParamCode(int addr, Type *type, vector<int> sizes){
    TAC::type = type;
    TAC::addr = addr;

    this->sizes = vector<int>();
    this->sizes.insert(this->sizes.end(), sizes.begin(), sizes.end());

    int tmp = 1;
    for(int i = sizes.size() - 1; i >= 0; i--){
        suffix.push_back(tmp);
        tmp *= sizes[i];
    }
}

class DecCode: public TAC{
    public:
        vector<int> suffix;
        vector<int> sizes;
        DecCode(int, Type *, string, vector<int>);
        string to_instruction();
};
DecCode::DecCode(int addr, Type *type, string name, vector<int> sizes){
    TAC::addr = addr;
    TAC::type = type;
    TAC::name = name;

    this->sizes = vector<int>();
    this->sizes.insert(this->sizes.end(), sizes.begin(), sizes.end());

    int tmp = 1;
    for(int i = sizes.size() - 1; i >= 0; i--){
        suffix.push_back(tmp);
        tmp *= sizes[i];
    }
}

class AssignCode: public TAC{
    public:
        int raddr;
        AssignCode(int, int, string);
        string to_instruction();
};
AssignCode::AssignCode(int addr, int raddr, string name){
    this->raddr = raddr;
    TAC::addr = addr;
    TAC::name = name;
}
string AssignCode::to_instruction(){
    char instruction[len];
    sprintf(instruction, "t%d := %s", TAC::addr, TAC::addr_to_string(raddr).c_str());
    return instruction;
}

class AssignToAddrCode: public TAC{
    public:
        int raddr;
        AssignToAddrCode(int,int);
        string to_instruction();
};
AssignToAddrCode::AssignToAddrCode(int addr, int raddr){
    this->raddr = raddr;
    TAC::addr = addr;
}
string AssignToAddrCode::to_instruction(){
    char instruction[len];
    sprintf(instruction, "*t%d := t%d", TAC::addr, raddr);
    return instruction;
}

class LabelCode:public TAC{
    public:
        LabelCode(int);
        string to_instruction();
};
LabelCode::LabelCode(int addr){
    TAC::addr = addr;
}
string LabelCode::to_instruction(){
    char instruction[len];
    sprintf(instruction, "LABEL label%d :", TAC::addr);
    return instruction;
}

class GotoCode: public TAC{
    public: 
        int *label;
        GotoCode(int, int *);
        string to_instruction();
};
GotoCode::GotoCode(int addr, int *label){
    this->label = label;
    TAC::addr = addr;
}
string GotoCode::to_instruction(){
    char instruction[len];
    sprintf(instruction, "GOTO label%d", *label);
    return instruction;
}

class IfCode: public TAC{
    public:
        string token;
        int *label;
        int laddr;
        int raddr;
        IfCode(int, int, int, string, int *);
        string to_instruction();
};
IfCode::IfCode(int addr, int laddr, int raddr, string token, int *label){
    this->laddr = laddr;
    this->raddr = raddr;
    this->token = token;
    this->label = label;
    TAC::addr = addr;
}
string IfCode::to_instruction(){
    char instruction[len];
    sprintf(instruction, "IF t%d %s t%d GOTO label%d", laddr, token.c_str(), raddr, *label);
    return instruction;
}

class ArithCode: public TAC{
    public:
        string token;
        int laddr;
        int raddr;
        ArithCode(int, int, int, string);
        string to_instruction();
};
ArithCode::ArithCode(int addr, int laddr, int raddr, string token){
    this->token = token;
    this->laddr = laddr;
    this->raddr = raddr;
    TAC::addr = addr;
}

class ReadCode: public TAC{
    public:
        ReadCode(int);
        string to_instruction();
};
ReadCode::ReadCode(int addr){
    TAC::addr = addr;
}
string ReadCode::to_instruction(){
    char instruction[len];
    sprintf(instruction, "READ t%d", TAC::addr);
    return instruction;
}

class AssignAddrCode: public TAC{
    public:
        int raddr;
        AssignAddrCode(int, int);
        string to_instruction();
};
AssignAddrCode::AssignAddrCode(int addr, int raddr){
    this->raddr = raddr;
    TAC::addr = addr;
}
string AssignAddrCode::to_instruction(){
    char instruction[len];
    sprintf(instruction, "t%d := &t%d", TAC::addr, raddr);
    return instruction;
}

class AssignValCode: public TAC{
    public:
        int raddr;
        AssignValCode(int, int);
        string to_instruction();
};
AssignValCode::AssignValCode(int addr, int raddr){
    this->raddr = raddr;
    TAC::addr = addr;
}
string AssignValCode::to_instruction(){
    char instruction[len];
    sprintf(instruction, "t%d := *t%d", TAC::addr, raddr);
    return instruction;
}

class ReturnCode: public TAC{
    public:
        int raddr;
        ReturnCode(int, int);
        string to_instruction();
};
ReturnCode::ReturnCode(int addr, int raddr){
    this->raddr = raddr;
    TAC::addr = addr;
}
string ReturnCode::to_instruction(){
    char instruction[len];
    sprintf(instruction, "RETURN %s", TAC::addr_to_string(raddr).c_str());
    return instruction;    
}

class WriteCode: public TAC{
    public: 
        int raddr;
        WriteCode(int, int);
        string to_instruction();
};
WriteCode::WriteCode(int addr, int raddr){
    this->raddr = raddr;
    TAC::addr = addr;
}
string WriteCode::to_instruction(){
    char instruction[len];
    sprintf(instruction, "WRITE t%d", raddr);
    return instruction;      
}

vector<TAC *> tac_list;
map<string, int> name_to_addr;

int * new_label(int addr = tac_list.size() + 1);
void init();
void back_patch(int addr, int true_list, int false_list);
void back_loop_patch(vector<int> *addrs, int last, int to);
void translate_ExtDefList(Node *node);
void translate_Programe(Node *root);
int translate_Exp(Node *node, bool single=false);
Type* translate_StructSpecifier(Node *node);
Type* translate_Specifier(Node *node);
TAC* translate_VarDec(Node *node, Type *type);
void translate_ParamDec(Node *node);
void translate_VarList(Node *node);
void translate_FunDec(Node *node, Type *type);
void translate_Dec(Node *node, Type *type);
void translate_DecList(Node *node, Type *type);
void translate_Def(Node *node);
vector<int> translate_Args(Node *node);
void translate_Stmt(Node *node);
void translate_StmtList(Node *node);
void translate_DefList(Node *node);
void translate_CompSt(Node *node);
void translate_ExtDecList(Node *node, Type *type);
void translate_ExtDef(Node *node);