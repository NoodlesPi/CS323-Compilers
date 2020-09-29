#include "linked_list.h"

node *linked_list_init(){
    node *head = (node*)malloc(sizeof(node));
    head->count = 0;
    head->next = NULL;
    return head;
}

void linked_list_free(node *head){
    node *cur = head;
    node *last;
    while(cur != NULL){
        last = cur;
        cur = cur->next;
        free(last);
    }
}

char linked_list_string[0x10000];

char *linked_list_tostring(node *head){
    node *cur = head->next;
    char *position;
    int length = 0;
    while(cur != NULL){
        position = linked_list_string + length;
        length += sprintf(position, "%d", cur->value);
        cur = cur->next;
        if(cur != NULL){
            position = linked_list_string + length;
            length += sprintf(position, "->");
        }
    }
    position = linked_list_string + length;
    length += sprintf(position, "%c", '\0');
    return linked_list_string;
}

int linked_list_size(node *head){
    return head->count;
}

void linked_list_append(node *head, int val){
    node *cur = head;
    node *new_node;
    while(cur->next != NULL){
        cur = cur->next;
    }
    new_node = (node*)malloc(sizeof(node));
    new_node->value = val;
    new_node->next = NULL;
    cur->next = new_node;
    head->count++;
}

/* your implementation goes here */
void linked_list_insert(node *head, int val, int index){
	if (index < 0 || index > head->count) return;
	node *cur = head;
	node *new_node;
	while( index > 0) {
		cur = cur->next;
		index--;
	}
	new_node = (node*)malloc(sizeof(node));
	new_node->value = val;
	new_node->next = cur->next;
	cur->next = new_node;
	head->count++;
}

void linked_list_delete(node *head, int index){
	if (index < 0 || index >= head->count) return;
	node *cur = head;
        while(index > 0){
	  cur = cur->next;
	  index--;
	}
	node *next = cur->next;
	node *last = next->next;
	cur->next = last;
 	free(next);
	head->count--;	
}

void linked_list_remove(node *head, int val){
	node *cur = head;
	node *pre;
	while(cur->next != NULL){
		pre = cur;
		cur = cur->next;
		if (cur->value == val) {
			pre->next = cur->next;
			free(cur);
			head->count--;
			return;
		}
	}
}

void linked_list_remove_all(node *head, int val){
	node *cur = head;
	node  *pre;
	while(cur->next != NULL){
		pre = cur;
		cur = cur->next;
		if (cur->value == val) {
			pre->next = cur->next;
			head->count--;
			cur = pre;
		}
	}
}

<<<<<<< HEAD
int linked_list_get(node *head, int index){
	if (index < 0 || index >= head->count) return -0x80000000;
	node *cur = head;
	while(index >= 0){
	    cur = cur->next;
	    index--;
	}
	return cur->value;

}

int linked_list_search(node *head, int val){
	int index = 0;
	node *cur = head;
	while(cur->next != NULL){
		cur = cur->next;
		if (cur->value == val){
			return index;
		}
		index++;
	}
	return -1;
}

node *linked_list_search_all(node *head, int val) {
	node *rst_head = linked_list_init();
	node *rst_cur = rst_head;
	node *cur = head;
	int index = 0;
	while(cur->next != NULL){
		cur = cur->next;
		if (cur->value == val) {
		   node *target = (node*)malloc(sizeof(node));
		   target->value = index;
		   target->next = NULL;

		   rst_cur->next = target;
		   rst_cur = rst_cur->next;
		   rst_head->count++;
		}
		index++;
	}
	return rst_head;
=======
void linked_list_insert(node *head, int val, int index){

}

void linked_list_delete(node *head, int index){

}

void linked_list_remove(node *head, int val){

}

void linked_list_remove_all(node *head, int val){

}

int linked_list_get(node *head, int index){
    return 0;
}

int linked_list_search(node *head, int val){
    return 0;
}

node *linked_list_search_all(node *head, int val){
    return linked_list_init();
>>>>>>> 442326270a6d942bf71b96cb5dac61e216b89457
}
