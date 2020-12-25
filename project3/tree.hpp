#include <bits/stdc++.h>

using namespace std;

extern char *yytext;
FILE *fout;

class Node {
    public:
        string token;
        string value;
        int line_num;
        int child_num;
        vector<Node *> child_list;

        Node(string, string, int, int, ...);
        void print_tree(int);
        bool is_terminal();
};

void print_indent(int);