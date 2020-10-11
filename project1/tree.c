#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "tree.h"

extern FILE *fout;

Node *newInternalNode(char *name, int cnt, ...) {
    Node *node = (Node*)malloc(sizeof(Node));
    node->name = name;
    node->is_token = 0;
    node->next = NULL;
    node->sons = initList();

    if (cnt < 0) return node;

    va_list children;
    va_start(children, cnt);

    Node *son = va_arg(children, Node*);
    list_append(node->sons, son);
    node->lineno = son->lineno;
    for(int i = 0; i < cnt - 1; i++) {
        list_append(node->sons, va_arg(children, Node*));
    }
    printf("lineno:%d name:%s\n", node->lineno, node->name);
    va_end(children);
    return node;
}

Node *newLeafNode(char *name, int lineno) {
        Node *node = (Node*)malloc(sizeof(Node));
        node->lineno = lineno;
        node->name = name;
        node->next = NULL;
        node->sons = NULL;
        node->is_token = 1;
    printf("lineno:%d name:%s\n", node->lineno, node->name);
        if (!strcmp(node->name, "INT")) {
            if (strlen(yytext) > 2 && (yytext[1] == 'x' || yytext[1] == 'X')) {
                node->int_val = strtol(yytext, NULL, 16);
            } else {
                node->int_val = atoi(yytext);
            }
        } else if (!strcmp(node->name, "FLOAT")) {
            node->float_val = atof(yytext);
        } else {
            char *tmp = (char *)malloc(sizeof(char) * strlen(yytext));
            strcpy(tmp, yytext);
            node->string_val = tmp;
        }
        return node;
}

void printSyntaxTree(Node *root, int indent) {
    if (root == NULL) return;
    if (strcmp(root->name, "Empty") != 0) {
        if (indent > 0) fprintf(fout, "%*c", indent * 2, ' ');

        fprintf(fout, "%s", root->name);

        if (!strcmp(root->name, "ID") || !strcmp(root->name, "TYPE") || !strcmp(root->name, "CHAR")) {
            fprintf(fout, ": %s\n", root->string_val);
        } else if (!strcmp(root->name, "INT")) {
            fprintf(fout, ": %d\n", root->int_val);
        } else if (!strcmp(root->name, "FLOAT")) {
            fprintf(fout, ": %f\n", root->float_val);
        } else if (root->is_token == 0) {
            fprintf(fout, " (%d)\n", root->lineno);
        } else {
            fprintf(fout, "\n");
        }
    }
    if (root->sons != NULL) {
        Node *cur = root->sons;
        while(cur->next != NULL) {
            printSyntaxTree(cur->next, indent + 1);
            cur = cur->next;
        }
    }
}

Node *initList() {
    Node *node = (Node*)malloc(sizeof(Node));
    node->next = NULL;
    node->sons = NULL;
    return node;
}

void list_append(Node *head, Node *target) {
    Node *cur = head;
    while(cur->next != NULL) {
        cur = cur->next;
    }
    cur->next = target;
}

Node *newNode(char *name, int lineno) {
    Node *node = (Node*)malloc(sizeof(Node));
    node->sons = NULL;
    node->next = NULL;
    node->name = name;
    node->lineno = lineno;
    return node;
}

/*int main() {
    Node *Program = newNode("Program", 1);
    Node *ExtDefList = newNode("ExtDefList", 1);
    Node *ExtDef = newNode("ExtDef", 1);
    Node *Specifier = newNode("Specifier", 1);
    Node *FunDec = newNode("FunDec", 1);
    Node *ComSt = newNode("ComSt", 2);
    Node *Type = newNode("Type",1);
    Type->string_val = "int";
    Node *ID = newNode("ID", 1);
    ID->string_val = "test_spl";
    Node *LP = newNode("LP", 1);
    Node *RP = newNode("RP", 1);
    Node *LC = newNode("LC",2);
    Node *RC = newNode("RC",4);
    Node *DefList = newNode("DefList",3);
    Node *StmtList = newNode("StmtList", 3);

    Program->sons = initList();
    list_append(Program->sons, ExtDefList);

    ExtDefList->sons = initList();
    list_append(ExtDefList->sons, ExtDef);

    ExtDef->sons = initList();
    list_append(ExtDef->sons, Specifier);
    list_append(ExtDef->sons, FunDec);
    list_append(ExtDef->sons, ComSt);

    Specifier->sons = initList();
    list_append(Specifier->sons, Type);

    FunDec->sons = initList();
    list_append(FunDec->sons, ID);
    list_append(FunDec->sons, LP);
    list_append(FunDec->sons, RP);

    ComSt->sons = initList();
    list_append(ComSt->sons, LC);
    list_append(ComSt->sons, DefList);
    list_append(ComSt->sons, StmtList);
    list_append(ComSt->sons, RC);

    printSyntaxTree(Program, 0);
}*/