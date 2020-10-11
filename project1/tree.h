#ifndef _TREE_H
#define _TREE_H

extern char *yytext;

typedef struct Node{
    int lineno;
    char *name;
    
    struct Node *next;
    struct Node *sons;

    union {
        int int_val;
        float float_val;
        char *string_val;
    };

    short is_token;

}Node;

// 内部节点
Node *newInternalNode(char *name, int cnt, ...);

// 叶子节点
Node *newLeafNode(char *name, int lineno);

// 打印语法树
void printSyntaxTree(Node *root, int indent);

// 初始化链表
Node *initList();

// 链表增加
void list_append(Node *head, Node *target);


#endif