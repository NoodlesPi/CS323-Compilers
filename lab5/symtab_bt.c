#include "symtab.h"

/*
 * symbol table type, binary tree impl
 */
struct symtab {
    entry entry;
    struct symtab *left, *right;
};

// ************************************************************
//    Your implementation goes here
// ************************************************************

symtab *symtab_init(){
    symtab* self = malloc(sizeof(symtab));
    memset(self, '\0', sizeof(symtab));
    self->left = NULL;
    self->right = NULL;
    return self;
}

int symtab_insert(symtab *self, char *key, VAL_T value){
    if(strcmp(key, self->entry.key) > 0){
        if(self->right == NULL){
            self->right = symtab_init();
            entry_init(&self->right->entry, key, value);
            return 1;
        }
        return symtab_insert(self->right, key, value);
    } else if(strcmp(key, self->entry.key) < 0){
        if(self->left == NULL){
            self->left = symtab_init();
            entry_init(&self->left->entry, key, value);
            return 1;
        }
        return symtab_insert(self->left, key, value);
    }else{
        return 0;
    }
    return 1;
}

VAL_T symtab_lookup(symtab *self, char *key){
    if(strcmp(key, self->entry.key) > 0){
        if(self->right == NULL){   
            return -1;
        }
        return symtab_lookup(self->right, key);
    } else if(strcmp(key, self->entry.key) < 0){
        if(self->left == NULL){ 
            return -1;
        }
       return symtab_lookup(self->left, key);
    } else{
        return self->entry.value;
    }
    return -1;
}

symtab *find_successer(symtab *self) {
    if (self->left == NULL){
	 return self;
    }
    return find_successer(self->left);	
}

symtab* symtab_rm(symtab* root, char* key, int* return_val){
   if(root == NULL) return root;
   
   if(strcmp(key, root->entry.key) < 0){
       root->left = symtab_rm(root->left, key, return_val);
   } else if(strcmp(key, root->entry.key) > 0){
        root->right = symtab_rm(root->right, key, return_val);
   } else{
       if(root->left == NULL){
           symtab* temp = root->right;
           free(root);
           *return_val = 1;
           return temp;
       } else if(root->right == NULL){
           symtab* temp = root->left;
           free(root);
           *return_val = 1;
           return temp;
       }
       symtab* temp = find_successer(root->right);
       root->entry = temp->entry;
       root->right = symtab_rm(root->right, temp->entry.key, return_val);
   }
}

int symtab_remove(symtab *self, char *key){
    int return_val = 0;
    symtab_rm(self, key, &return_val);
    return return_val;
}