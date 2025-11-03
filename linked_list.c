#include "linked_list.h"
#include <stdlib.h>

Node* create_node(const Student* st) {
	Node* newNode = (Node*)malloc(sizeof * newNode);
	if (!newNode) {
		return 0;
	}

	newNode->s = *st;
	newNode->next = NULL;

	return newNode;
}
int insert_node(LinkedList* L, const Student* st) {
    Node* newNode = create_node(st);
    if (!newNode) {
        return -1;                    // allocation failed
    }

    if (!L->head) {                   // empty list: head and tail become newNode
        L->head = L->tail = newNode;
    } else {                          // append to tail
        L->tail->next = newNode;
        L->tail = newNode;
    }
    return 0;                         // success
}

void list_init(LinkedList* L) {
	L->head = L->tail = NULL;
}

void list_clear(LinkedList* L) {
	for (Node* p = L->head; p;) {
		Node* n = p->next;
		free(p);
		p = n;
	}
	L->head = L->tail = NULL;
}

Node* list_find_by_id(LinkedList* L, int id) {
	for (Node* p = L->head; p; p = p->next) {
		if (p->s.id == id) {
			return p;
		}
	}
	return NULL;
}

int list_delete_by_id(LinkedList* L, int id) {
	Node* prev = NULL;
	Node* cur = L->head;
	while (cur) {
		if (cur->s.id == id) {
			if (prev) {
				prev->next = cur->next;
			}
			else {
				L->head = cur->next;
			}
			free(cur);
			
			return 1;
		}
		prev = cur;
		cur = cur->next;
	}
	return 0;
}

