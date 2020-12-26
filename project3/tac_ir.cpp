#include "tac_ir.hpp"

// init all relative global variables
void init(){
    tac_list = vector<TAC *>();
    name_to_addr = map<string, int>();
    con_addr = vector<int>();
    br_addr = vector<int>();
}

int * new_label(int addr){
    return new int(addr);
}

void back_patch(int addr, int true_list, int false_list){
    if (tac_list[addr]->is_swap){
        swap(true_list, false_list);
    }
    *dynamic_cast<IfCode *>(tac_list[addr])->label = true_list;
    *dynamic_cast<GotoCode *>(tac_list[addr + 1])->label = false_list;
}

void back_loop_patch(vector<int> *addrs, int last, int to){
    while(addrs->size() > last){
        *dynamic_cast<GotoCode *>(tac_list[addrs->back()])->label = to;
        addrs->pop_back();
    }
}

/*
ExtDefList:
    ExtDef ExtDefList { $$ = new Node("ExtDefList", "", $1->line_num, 2, $1, $2); }
    | %empty { $$ = new Node("ExtDefList", "", yylineno, 0); }
    ;
*/
void translate_ExtDefList(Node *node){
    Node *tmp = node;
    while(tmp->child_num){
        translate_ExtDef(tmp->child_list[0]);
        tmp = tmp->child_list[1];
    }
}


/*
Program: 
    ExtDefList { root = new Node("Program", "", $1->line_num, 1, $1); }
    ;
*/
void translate_Programe(Node *root){
    init();
    translate_ExtDefList(root->child_list[0]);
}

/*
Exp:
    Exp ASSIGN Exp { $$ = new Node("Exp", "", $1->line_num, 3, $1, $2, $3); }
    | Exp AND Exp { $$ = new Node("Exp", "", $1->line_num, 3, $1, $2, $3); }
    | Exp OR Exp { $$ = new Node("Exp", "", $1->line_num, 3, $1, $2, $3); }
    | Exp LT Exp { $$ = new Node("Exp", "", $1->line_num, 3, $1, $2, $3); }
    | Exp LE Exp { $$ = new Node("Exp", "", $1->line_num, 3, $1, $2, $3); }
    | Exp GT Exp { $$ = new Node("Exp", "", $1->line_num, 3, $1, $2, $3); }
    | Exp GE Exp { $$ = new Node("Exp", "", $1->line_num, 3, $1, $2, $3); }
    | Exp NE Exp { $$ = new Node("Exp", "", $1->line_num, 3, $1, $2, $3); }
    | Exp EQ Exp { $$ = new Node("Exp", "", $1->line_num, 3, $1, $2, $3); }
    | Exp PLUS Exp { $$ = new Node("Exp", "", $1->line_num, 3, $1, $2, $3); }
    | Exp MINUS Exp { $$ = new Node("Exp", "", $1->line_num, 3, $1, $2, $3); }
    | Exp MUL Exp { $$ = new Node("Exp", "", $1->line_num, 3, $1, $2, $3); }
    | Exp DIV Exp { $$ = new Node("Exp", "", $1->line_num, 3, $1, $2, $3); }
    
    | LP Exp RP { $$ = new Node("Exp", "", $1->line_num, 3, $1, $2, $3); }
    | MINUS Exp  { $$ = new Node("Exp", "", $1->line_num, 2, $1, $2); }
    | NOT Exp  { $$ = new Node("Exp", "", $1->line_num, 2, $1, $2); }
    
    | ID LP Args RP { $$ = new Node("Exp", "", $1->line_num, 4, $1, $2, $3, $4); }
    | ID LP RP { $$ = new Node("Exp", "", $1->line_num, 3, $1, $2, $3); }
    
    | Exp LB Exp RB { $$ = new Node("Exp", "", $1->line_num, 4, $1, $2, $3, $4); }
    | Exp DOT ID { $$ = new Node("Exp", "", $1->line_num, 3, $1, $2, $3); }
    
    | ID  { $$ = new Node("Exp", "", $1->line_num, 1, $1); }
    | INT { $$ = new Node("Exp", "", $1->line_num, 1, $1); }
    | FLOAT{ $$ = new Node("Exp", "", $1->line_num, 1, $1); }
    | CHAR{ $$ = new Node("Exp", "", $1->line_num, 1, $1); }
    | READ LP RP { $$ = new Node("Exp", "", $1->line_num, 3, $1, $2, $3); }
    ;
*/
int translate_Exp(Node *node, bool single){
    // Exp ASSIGN Exp { $$ = new Node("Exp", "", $1->line_num, 3, $1, $2, $3); }
    if(node->child_list[1]->token == "ASSIGN"){
        int l = translate_Exp(node->child_list[0], true);
        int r = translate_Exp(node->child_list[2]);

        if(typeid(*tac_list[l])==typeid(AssignCode)){
            dynamic_cast<AssignCode *>(tac_list[l])->raddr = r;
        }else{
            dynamic_cast<AssignToAddrCode *>(tac_list[l])->raddr = r;
        }
        
        return r;
    }

    // Exp AND Exp { $$ = new Node("Exp", "", $1->line_num, 3, $1, $2, $3); }
    if(node->child_list[1]->token == "AND"){
        int l = translate_Exp(node->child_list[0]);
        int addr = tac_list.size();
        tac_list.push_back(new LabelCode(addr));
        bool ls = tac_list[l]->is_swap;

        int r = translate_Exp(node->child_list[2]);
        bool rs = tac_list[r]->is_swap;

        if(ls){
            *dynamic_cast<GotoCode *>(tac_list[l + 1])->label = addr;
            if (rs){//exp2.truelist
                dynamic_cast<IfCode *>(tac_list[l])->label = dynamic_cast<IfCode *>(tac_list[r])->label;
            }else{ //exp2.falselist
                dynamic_cast<IfCode *>(tac_list[l])->label = dynamic_cast<GotoCode *>(tac_list[r + 1])->label;
            } 
        }else{
            *dynamic_cast<IfCode *>(tac_list[l])->label = addr;
            if (rs){//exp2.truelist
                dynamic_cast<GotoCode *>(tac_list[l + 1])->label = dynamic_cast<IfCode *>(tac_list[r])->label;
            }else{ //exp2.falselist
                dynamic_cast<GotoCode *>(tac_list[l + 1])->label = dynamic_cast<GotoCode *>(tac_list[r + 1])->label;
            }
        }

        return r;
    }

    // Exp OR Exp { $$ = new Node("Exp", "", $1->line_num, 3, $1, $2, $3); }
    if(node->child_list[1]->token == "OR"){
        int l = translate_Exp(node->child_list[0]);
        int addr = tac_list.size();
        tac_list.push_back(new LabelCode(addr));
        bool ls = tac_list[l]->is_swap;

        int r = translate_Exp(node->child_list[2]);
        bool rs = tac_list[r]->is_swap;

        if(ls){
            *dynamic_cast<IfCode *>(tac_list[l])->label = addr;
            if (rs){//exp2.truelist
                dynamic_cast<GotoCode *>(tac_list[l + 1])->label = dynamic_cast<GotoCode *>(tac_list[r + 1])->label;
            }else{ //exp2.falselist
                dynamic_cast<GotoCode *>(tac_list[l + 1])->label = dynamic_cast<IfCode *>(tac_list[r])->label;
            } 
        }else{
            *dynamic_cast<GotoCode *>(tac_list[l + 1])->label = addr;
            if (rs){//exp2.truelist
                dynamic_cast<IfCode *>(tac_list[l])->label = dynamic_cast<GotoCode *>(tac_list[r + 1])->label;
            }else{ //exp2.falselist
                dynamic_cast<IfCode *>(tac_list[l])->label = dynamic_cast<IfCode *>(tac_list[r])->label;
            }
        }

        return r;
    }

    /*
    | Exp LT Exp { $$ = new Node("Exp", "", $1->line_num, 3, $1, $2, $3); }
    | Exp LE Exp { $$ = new Node("Exp", "", $1->line_num, 3, $1, $2, $3); }
    | Exp GT Exp { $$ = new Node("Exp", "", $1->line_num, 3, $1, $2, $3); }
    | Exp GE Exp { $$ = new Node("Exp", "", $1->line_num, 3, $1, $2, $3); }
    | Exp NE Exp { $$ = new Node("Exp", "", $1->line_num, 3, $1, $2, $3); }
    | Exp EQ Exp { $$ = new Node("Exp", "", $1->line_num, 3, $1, $2, $3); }
    */
    if(node->child_list[1]->token == "LT" || node->child_list[1]->token == "LE" || node->child_list[1]->token == "GT"
    || node->child_list[1]->token == "GE" || node->child_list[1]->token == "NE" || node->child_list[1]->token == "EQ"){
        int l = translate_Exp(node->child_list[0]);
        int r = translate_Exp(node->child_list[2]);

        int addr = tac_list.size();
        tac_list.push_back(new IfCode(addr, l, r, node->child_list[1]->token, new_label()));

        tac_list.push_back(new GotoCode(tac_list.size(), new_label()));
        return addr;
    }

    /*
    | Exp PLUS Exp { $$ = new Node("Exp", "", $1->line_num, 3, $1, $2, $3); }
    | Exp MINUS Exp { $$ = new Node("Exp", "", $1->line_num, 3, $1, $2, $3); }
    | Exp MUL Exp { $$ = new Node("Exp", "", $1->line_num, 3, $1, $2, $3); }
    | Exp DIV Exp { $$ = new Node("Exp", "", $1->line_num, 3, $1, $2, $3); }
    */
    if(node->child_list[1]->token == "PLUS" || node->child_list[1]->token == "MINUS" || 
    node->child_list[1]->token == "MUL" || node->child_list[1]->token == "DIV" ){
        int l = translate_Exp(node->child_list[0]);
        int r = translate_Exp(node->child_list[2]);

        int addr = tac_list.size();
        tac_list.push_back(new ArithCode(addr, l, r, node->child_list[1]->token));

        return addr;
    }

    // | READ LP RP { $$ = new Node("Exp", "", $1->line_num, 3, $1, $2, $3); }
    if(node->child_list[0]->token == "READ"){
        int addr = tac_list.size();
        tac_list.push_back(new ReadCode(addr));
        return addr;
    }

    /*
    | INT { $$ = new Node("Exp", "", $1->line_num, 1, $1); }
    | FLOAT{ $$ = new Node("Exp", "", $1->line_num, 1, $1); }
    | CHAR{ $$ = new Node("Exp", "", $1->line_num, 1, $1); }
    */
    if(node->child_list[0]->token == "INT" || node->child_list[0]->token == "FLOAT" || node->child_list[0]->token == "CHAR"){
        int addr = tac_list.size();
        tac_list.push_back(new AssignCode(addr, -atoi(node->child_list[0]->value.c_str()), ""));
        return addr;
    }

    // | MINUS Exp  { $$ = new Node("Exp", "", $1->line_num, 2, $1, $2); }
    if(node->child_list[0]->token == "MINUS"){
        int raddr = translate_Exp(node->child_list[1]);
        int addr = tac_list.size();
        tac_list.push_back(new ArithCode(addr, 0, raddr, "MINUS"));
        return addr;
    }

    // | NOT Exp  { $$ = new Node("Exp", "", $1->line_num, 2, $1, $2); }
    if(node->child_list[0]->token == "NOT"){
        int raddr = translate_Exp(node->child_list[1]);
        tac_list[raddr]->is_swap ^= true;
        return raddr;
    }

    // | ID  { $$ = new Node("Exp", "", $1->line_num, 1, $1); }
    if(node->child_list[0]->token == "ID"){
        int addr = name_to_addr[node->child_list[0]->value];
        if(single){
            if(!addr){
               addr = tac_list.size();
               name_to_addr[node->child_list[0]->value] = addr;
            }
            tac_list.push_back(new AssignCode(addr, 0, ""));
            return addr;
        }else if(!addr){

        }else{
            return addr;
        }
    }

    // | Exp LB Exp RB { $$ = new Node("Exp", "", $1->line_num, 4, $1, $2, $3, $4); }
    if(node->child_list[1]->token == "LB"){
        vector<Node *> exp_list = {node};
        int head;

        while(!exp_list.empty()){
            Node *tmp = exp_list.back();
            if(tmp->child_list[0]->token != "ID"){
                exp_list.push_back(tmp->child_list[0]);
                continue;
            }

            head = translate_Exp(tmp);
            exp_list.pop_back();

            vector<int> *suffix;
            if(typeid(*tac_list[head])==typeid(DecCode)){
                suffix = &(dynamic_cast<DecCode *>(tac_list[head])->suffix);
                tac_list.push_back(new AssignAddrCode(tac_list.size(), head));
                head = tac_list.size()-1;
            }else{
                suffix = &(dynamic_cast<ParamCode*>(tac_list[head])->suffix);
            }

            int _index = exp_list.size();
            while (_index--)
            {
                Node *tmp1 = exp_list.back();
                exp_list.pop_back();

                int offset = translate_Exp(tmp1->child_list[2]);
                tac_list.push_back(new ArithCode(tac_list.size(), offset, -(*suffix)[_index] * type_size, "MUL"));
                offset = tac_list.size() - 1;

                tac_list.push_back(new ArithCode(tac_list.size(), head, offset, "ADD"));
                head = tac_list.size()-1;
            } 
        }
        int addr = tac_list.size();
        if(single){
            tac_list.push_back(new AssignToAddrCode(head, 0));
            return addr;
        }

        tac_list.push_back(new AssignValCode(addr, head));
        return addr;
    }

    // | Exp DOT ID { $$ = new Node("Exp", "", $1->line_num, 3, $1, $2, $3); }
    if(node->child_list[1]->token == "DOT"){
        vector<Node *> exp_list = {node};
        int head;

        while(!exp_list.empty()){
            Node *tmp = exp_list.back();
            if(tmp->child_list[0]->token != "ID"){
                exp_list.push_back(tmp->child_list[0]);
                continue;
            }

            head = translate_Exp(tmp);
            Type *type = tac_list[head]->type;
            exp_list.pop_back();

            if(typeid(*tac_list[head])==typeid(DecCode)){
                tac_list.push_back(new AssignAddrCode(tac_list.size(), head));
                head = tac_list.size()-1;
            }

            int _index = exp_list.size();
            while (_index--)
            {
                Node *tmp1 = exp_list.back();
                exp_list.pop_back();

                string name = tmp1->child_list[2]->value;
                int offset = dynamic_cast<Structure *>(type)->get_offset(name);

                tac_list.push_back(new ArithCode(tac_list.size(), head, -offset, "ADD"));
                head = tac_list.size()-1;
            } 
        }
        int addr = tac_list.size();
        if(single){
            tac_list.push_back(new AssignToAddrCode(head, 0));
            return addr;
        }

        tac_list.push_back(new AssignValCode(addr, head));
        return addr;
    }
}
/*
StructSpecifier:
    STRUCT ID LC DefList RC { $$ = new Node("StructSpecifier", "", $1->line_num, 5, $1, $2, $3, $4, $5); }
    | STRUCT ID { $$ = new Node("StructSpecifier", "", $1->line_num, 2, $1, $2); }
    ;
*/
Type* translate_StructSpecifier(Node *node){
    return strc_map.find(node->child_list[1]->value)->second;
}

/*
Specifier:
    TYPE { $$ = new Node("Specifier", "", $1->line_num, 1, $1); }
    | StructSpecifier { $$ = new Node("Specifier", "", $1->line_num, 1, $1); }
    ;
*/
Type* translate_Specifier(Node *node){
    if(node->child_list[0]->token == "TYPE"){
        return new Primitive(node->token);
    }
    return translate_StructSpecifier(node->child_list[0]);
}

/*
VarDec:
    ID { $$ = new Node("VarDec", "", $1->line_num, 1, $1); }
    | VarDec LB INT RB { $$ = new Node("VarDec", "", $1->line_num, 4, $1, $2, $3, $4); }
    ;
*/
TAC* translate_VarDec(Node *node, Type *type){
    vector<int> sizes = vector<int>();
    vector<Node *> vardec_list = {node};

    string name;
    while(!vardec_list.empty()){
        Node *tmp = vardec_list.back();
        if(tmp->child_list[0]->token != "ID"){
            vardec_list.push_back(tmp->child_list[0]);
            continue;
        }

        vardec_list.pop_back();
        name = tmp->child_list[0]->value;
        while(!vardec_list.empty()){
            Node *tmp1 = vardec_list.back();
            sizes.push_back(atoi(tmp1->child_list[2]->value.c_str()));
            vardec_list.pop_back();
        }
    }

    if((typeid(*type)==typeid(Structure)) || sizes.size()){
        return new DecCode(tac_list.size(), type, name, sizes);
    }

    return new AssignCode(tac_list.size(), 0, name);

}

/*
ParamDec:
    Specifier VarDec { $$ = new Node("ParamDec", "", $1->line_num, 2, $1, $2); }
    ;
*/
void translate_ParamDec(Node *node){
    Type *type = translate_Specifier(node->child_list[0]);
    TAC *tac = translate_VarDec(node->child_list[1], type);

    int addr = tac_list.size();
    name_to_addr[tac->name] = addr;

    if(typeid(*tac)==typeid(AssignCode)){
        tac_list.push_back(new ParamCode(addr, type, {}));
    }else{
        tac_list.push_back(new ParamCode(addr, type, dynamic_cast<DecCode *>(tac)->sizes));
    }
}

/*
VarList:
    ParamDec COMMA VarList { $$ = new Node("VarList", "", $1->line_num, 3, $1, $2, $3); }
    | ParamDec { $$ = new Node("VarList", "", $1->line_num, 1, $1); }
    ;
*/
void translate_VarList(Node *node){
    vector<Node *> var_list = {node->child_list[0]};

    Node *tmp = node;
    while(tmp->child_num > 1){
        tmp = tmp->child_list[2];
        var_list.push_back(tmp->child_list[0]);
    }

    while(!var_list.empty()){
        translate_ParamDec(var_list.back());
        var_list.pop_back();
    }
}

/*
FunDec:
    ID LP VarList RP { $$ = new Node("FunDec", "", $1->line_num, 4, $1, $2, $3, $4); }
    | ID LP RP { $$ = new Node("FunDec", "", $1->line_num, 3, $1, $2, $3); }
    ;
*/
void translate_FunDec(Node *node, Type *type){
    string name = node->child_list[0]->value;
    int addr = tac_list.size();
    name_to_addr[name] = addr;
    tac_list.push_back(new FuncCode(name, addr));

    if(node->child_list[2]->token == "VarList"){
        translate_VarList(node->child_list[2]);
    }
}

/*
Dec:
    VarDec { $$ = new Node("Dec", "", $1->line_num, 1, $1); }
    | VarDec ASSIGN Exp { $$ = new Node("Dec", "", $1->line_num, 3, $1, $2, $3); }
    ;
*/
void translate_Dec(Node *node, Type *type){
    int addr = 0;
    if(node->child_num > 1){
        addr = translate_Exp(node->child_list[2]);
    }

    TAC *tac = translate_VarDec(node->child_list[0], type);
    if(addr){
        dynamic_cast<AssignCode *>(tac)->raddr = addr;
    }
    name_to_addr[tac->name] = tac_list.size();
    tac_list.push_back(tac);
}

/*
DecList:
    Dec { $$ = new Node("DecList", "", $1->line_num, 1, $1); }
    | Dec COMMA DecList { $$ = new Node("DecList", "", $1->line_num, 3, $1, $2, $3); }
    ;
*/
void translate_DecList(Node *node, Type *type){
    translate_Dec(node->child_list[0], type);
    Node *tmp = node;
    while(tmp->child_num > 1){
        tmp = tmp->child_list[2];
        translate_Dec(node->child_list[0], type);
    }
}

/*
Def:
    Specifier DecList SEMI { $$ = new Node("Def", "", $1->line_num, 3, $1, $2, $3); }
    ;
*/
void translate_Def(Node *node){
    translate_DecList(node->child_list[1], translate_Specifier(node->child_list[0]));
}

/*
Args:
    Exp COMMA Args { $$ = new Node("Args", "", $1->line_num, 3, $1, $2, $3); }
    | Exp { $$ = new Node("Args", "", $1->line_num, 1, $1); }
    ;
*/
vector<int> translate_Args(Node *node){
    vector<int> args = vector<int>();
    int expid = translate_Exp(node->child_list[0]);
    if (typeid(*tac_list[expid])==typeid(DecCode)){
        tac_list.push_back(new AssignCode(tac_list.size(), expid, ""));
        expid = tac_list.size() - 1;
    }
    args.push_back(expid);

    Node *tmp = node;
    while (tmp->child_num > 1)
    {
        tmp = tmp->child_list[2];
        expid = translate_Exp(tmp->child_list[0]);
        args.push_back(expid);
        if (typeid(*tac_list[expid])!=typeid(DecCode)){
            tac_list.push_back(new AssignAddrCode(tac_list.size(), expid));
            expid = tac_list.size() - 1;
        }
    }
    return args;
}

/*
Stmt:
    Exp SEMI { $$ = new Node("Stmt", "", $1->line_num, 2, $1, $2); }
    | CompSt { $$ = new Node("Stmt", "", $1->line_num, 1, $1); }
    | RETURN Exp SEMI { $$ = new Node("Stmt", "", $1->line_num, 3, $1, $2, $3); }

    | IF LP Exp RP Stmt { $$ = new Node("Stmt", "", $1->line_num, 5, $1, $2, $3, $4, $5); }
    | IF LP Exp RP Stmt ELSE Stmt { $$ = new Node("Stmt", "", $1->line_num, 7, $1, $2, $3, $4, $5, $6, $7); }
    | WHILE LP Exp RP Stmt { $$ = new Node("Stmt", "", $1->line_num, 5, $1, $2, $3, $4, $5); }

    | WRITE LP Args RP SEMI { $$ = new Node("Stmt", "", $1->line_num, 5, $1, $2, $3, $4, $5); }
    ;
*/
void translate_Stmt(Node *node){
    if(node->child_list[0]->token == "Exp"){
        translate_Exp(node->child_list[0]);
    }else if(node->child_list[0]->token == "CompSt"){
        translate_CompSt(node->child_list[0]);
    }else if(node->child_list[0]->token == "RETURN"){
        tac_list.push_back(new ReturnCode(tac_list.size(), translate_Exp(node->child_list[1])));
    }else if(node->child_list[0]->token == "IF"){
        int exp_addr = translate_Exp(node->child_list[2]);
        int true_list = tac_list.size();
        tac_list.push_back(new LabelCode(true_list));

        translate_Stmt(node->child_list[4]);

        int jump_addr;
        if(node->child_num > 5){
            jump_addr = tac_list.size();
            tac_list.push_back(new GotoCode(jump_addr, new_label()));
        }
        int false_list = tac_list.size();
        back_patch(exp_addr, true_list, false_list);
        if(node->child_num > 5){
            translate_Stmt(node->child_list[6]);
            int jump_to = tac_list.size();
            tac_list.push_back(new LabelCode(jump_to));
            *dynamic_cast<GotoCode *>(tac_list[jump_addr])->label = jump_to;
        }

    }else if(node->child_list[0]->token == "WHILE"){
        int contop = con_addr.size();
        int brtop = br_addr.size();

        int loop_start = tac_list.size();
        tac_list.push_back(new LabelCode(loop_start));

        int exp_addr = translate_Exp(node->child_list[2]);

        int true_list = tac_list.size();
        tac_list.push_back(new LabelCode(true_list));

        translate_Stmt(node->child_list[4]);

        int loop_back = tac_list.size();
        tac_list.push_back(new GotoCode(loop_back, new_label(loop_start)));


        int false_list = tac_list.size();
        tac_list.push_back( new LabelCode(false_list));
        
        back_patch(exp_addr, true_list, false_list);
        back_loop_patch(&con_addr, contop, loop_start);
        back_loop_patch(&br_addr, brtop, false_list);
    }else if(node->child_list[0]->token == "WRITE"){
        auto res = translate_Args(node->child_list[2]);
        for (auto it : res){
            tac_list.push_back(new WriteCode(tac_list.size(), it));
        }
    }
}

/*
Stmt:
StmtList:
    Stmt StmtList { $$ = new Node("StmtList", "", $1->line_num, 2, $1, $2); }
    | %empty { $$ = new Node("Empty", "", yylineno, 0); }
    ;
*/
void translate_StmtList(Node *node){
    Node *tmp = node;
    while(tmp->child_num){
        translate_Stmt(tmp->child_list[0]);
        tmp = tmp->child_list[1];
    }
}

/*
DefList:
    Def DefList { $$ = new Node("DefList", "", $1->line_num, 2, $1, $2); }
    | %empty { $$ = new Node("Empty", "", yylineno, 0); }
    ;
*/
void translate_DefList(Node *node){
    Node *tmp = node;
    while(tmp->child_num){
        handle_Def(tmp->child_list[0]);
        tmp = tmp->child_list[1];
    }
}

/*
CompSt:
    LC DefList StmtList RC { $$ = new Node("CompSt", "", $1->line_num, 4, $1, $2, $3, $4); }
    ;
*/
void translate_CompSt(Node *node){
    translate_DefList(node->child_list[1]);
    translate_StmtList(node->child_list[2]);
}

/*
ExtDecList:
    VarDec { $$ = new Node("ExtDecList", "", $1->line_num, 1, $1); }
    | VarDec COMMA ExtDecList { $$ = new Node("ExtDecList", "", $1->line_num, 3, $1, $2, $3); }
    ;
*/
void translate_ExtDecList(Node *node, Type *type){
    TAC *tac = translate_VarDec(node->child_list[0], type);
    Node *tmp = node;
    while(tmp->child_num > 1){
        tmp = tmp->child_list[2];
        translate_VarDec(tmp->child_list[0], type);
    }

    name_to_addr[tac->name] = tac_list.size();
    tac_list.push_back(tac);
}

/*
ExtDef:
    Specifier ExtDecList SEMI { $$ = new Node("ExtDef", "", $1->line_num, 3, $1, $2, $3); }
    | Specifier SEMI { $$ = new Node("ExtDef", "", $1->line_num, 2, $1, $2); }
    | Specifier FunDec CompSt { $$ = new Node("ExtDef", "", $1->line_num, 3, $1, $2, $3); }
    ;
*/
void translate_ExtDef(Node *node){
    Type *type = translate_Specifier(node->child_list[0]);
    if(node->child_list[1]->token == "FunDec"){
        translate_FunDec(node->child_list[1], type);
        translate_CompSt(node->child_list[2]);
    }else if(node->child_list[1]->token == "ExtDecList"){
        translate_ExtDecList(node->child_list[1], type);
    }
}
