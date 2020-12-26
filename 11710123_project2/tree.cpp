#include "tree.h"
#include <stdarg.h>
using namespace std;
extern FILE* fout;

Node::Node(int type, int symbol_type, string value, int lineno, int child_num, ...)
    :type(type), symbol_type(symbol_type), value(value), 
    child_num(child_num), lineno(lineno)
{
    children = vector<Node*>();
    
    va_list args;
    va_start(args, child_num);
    while(child_num--){
        Node* node = va_arg(args, Node*);
        Node* new_node = new Node(*node);
    
        this->children.push_back(new_node);
    }
    va_end(args);
}
