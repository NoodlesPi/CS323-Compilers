#include "tree.hpp"

using namespace std;

// 判断token是否是终结符
bool Node::is_terminal(){
    if(token == "Program" || token == "ExtDefList" || token == "ExtDef" || token == "ExtDecList"
    || token == "Specifier" || token == "StructSpecifier" || token == "VarDec" || token == "VarList"
    || token == "FunDec" || token == "ParamDec" || token == "CompSt" || token == "Stmt" || token == "StmtList"
    || token == "Condition" || token == "DefList" || token == "Def" || token == "DecList" || token == "Dec"
    || token == "Exp" || token == "Args") return false;
    return true;
}

Node::Node(string token, string value, int line_num, int child_num, ...){
    this->token = token;
    this->value = value;
    this->line_num = line_num;
    this->child_num = child_num;
    va_list args;
    va_start(args, child_num);
    while(child_num--){
        this->child_list.push_back(va_arg(args, Node *));
    }
    va_end(args);
}

// 以该节点为root打印语法树
void Node::print_tree(int indent){
    if(is_terminal()){
        print_indent(indent);
        if(token == "INT" || token == "FLOAT" || token == "CHAR" || token == "ID" || token == "TYPE"){
            fprintf(fout, "%s: %s\n", token.c_str(), value.c_str());
        }else{
            fprintf(fout, "%s\n", token.c_str());
        }
    }else{
        if(!child_list.empty()){
            print_indent(indent);
            fprintf(fout, "%s (%d)\n", token.c_str(), line_num);
        }
    }
    for(Node * n : child_list) {
        n->print_tree(indent + 2);
    }
}

// 根据入参打印空白
void print_indent(int indent){
    string blank = " ";
    while(indent--){
        blank += " ";
    }
    fprintf(fout, "%s", blank.c_str());
}
