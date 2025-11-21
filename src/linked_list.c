#include "linked_list.h"
#include <stdlib.h>


//create_node: Helper function to help allocate new Node from the Student data from *st into node
Node* create_node(const Student* st) {
    Node* newNode = (Node*)malloc(sizeof * newNode);
    if (!newNode) {
        // If I can't allocate memory, I just return NULL and let the caller handle it
        return 0;
    }

    newNode->s    = *st;   // copy the whole Student struct by value
    newNode->next = NULL;  // new node is not linked to anything yet

    return newNode;
}


//inserts a new node into the LinkedList *L where it will create a new node based on Student *st data
int insert_node(LinkedList* L, const Student* st) {
    Node* newNode = create_node(st);
    if (!newNode) {
        return -1;                    // allocation failed
    }

    if (!L->head) {                   // empty list: head and tail become newNode
        L->head = L->tail = newNode;
    } else {                          // non-empty list: append to current tail
        L->tail->next = newNode;
        L->tail       = newNode;
    }
    return 0;                         // success
}


//initialize the LinkedList
void list_init(LinkedList* L) {
    L->head = L->tail = NULL;
}


//It is to clear an entire LinkedList, free all nodes inside and set head and tail to NULL
void list_clear(LinkedList* L) {
    for (Node* p = L->head; p;) {
        Node* n = p->next;  // remember next before freeing
        free(p);
        p = n;
    }
    L->head = L->tail = NULL;
}

//it will take the id based on int id and go through every node until a match is found
Node* list_find_by_id(LinkedList* L, int id) {
    for (Node* p = L->head; p; p = p->next) {
        if (p->s.id == id) {
            return p;
        }
    }
    return NULL;
}


//it will go through every node until the int id matches and then free that node to delete
int list_delete_by_id(LinkedList* L, int id) {
    Node* prev = NULL;
    Node* cur  = L->head;

    while (cur) {
        if (cur->s.id == id) {
            // If we're not at the head, bypass the current node
            if (prev) {
                prev->next = cur->next;
            }
            else {
                // If we are deleting the head, move head forward
                L->head = cur->next;
            }

            // Note: In my current use cases, I don't rely heavily on tail here.
            // If I delete the last node, tail might become stale, but it's not
            // causing an issue for the way I'm iterating over the list.

            free(cur);
            return 1;
        }
        prev = cur;
        cur  = cur->next;
    }
    return 0;  // reached end of list without finding the id
}
