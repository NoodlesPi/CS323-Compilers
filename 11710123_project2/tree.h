#include <iostream>
#include <vector>
using namespace std;

enum Nonterminal{
    Program, 
    ExtDefList, ExtDef, ExtDecList,
    Specifier, StructSpecifier,
    VarDec, FunDec, VarList, ParamDec,
    CompSt, Stmt, StmtList,
    ForDef, ForInc, ForCon, Condition,
    DefList, Def, DecList, Dec,
    Exp, Args
};

enum Token{
    T_INT, T_FLOAT, T_CHAR, 
    T_ADD, T_SUB, T_MUL, T_DIV, T_EQ, 
    T_BITAND, T_BITOR, T_BITWISE, T_BITXOR,
    T_ASSIGN, T_AND, T_OR, T_LE, T_LT, T_GE, T_GT, T_NE, T_NOT, 
    T_STRUCT, T_SEMI, T_COMMA,
    T_LC, T_RC, T_LB, T_RB, T_LP, T_RP,
    T_RETURN, T_IF, T_ELSE, T_WHILE, T_FOR, 
    T_ID, T_TYPE, 
    T_DOT,
    T_BREAK, T_CONTINUE
};

enum SYMBOL_TYPE{
    NONTERMINAL, TOKEN, STANDARD
};

class Node{
public:
    int type;
    string value;
    int child_num;
    int symbol_type;
    int lineno;
    vector<Node*> children;

    Node(int type, int symbol_type, string value, int lineno, int child_num, ...);

};
