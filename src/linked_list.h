#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stddef.h>


#define MAX_NAME 50
#define MAX_PROGRAM 50
#define MAX_STUDENTS 100

typedef struct { //Student Structure
    int id;
    char name[MAX_NAME];
    char programme[MAX_PROGRAM];
    float mark;
} Student;

typedef struct Node { //Student Node
    Student s;
    struct Node* next;
} Node;

typedef struct { //LinkedList structure
    Node* head;
    Node* tail;
} LinkedList;

//typedef enum { //Sorting Enumerate
//    SORT_BY_ID,
//    SORT_BY_NAME,
//    SORT_BY_PROGRAMME,
//    SORT_BY_MARK
//} SortKey;
//
//int accending: 1 = ascending, 0 = descending

Node* create_node(const Student* st);
void list_init(LinkedList* L);
void list_clear(LinkedList* L);
int insert_node(LinkedList* L, const Student* st);
Node* list_find_by_id(LinkedList* L, int id);
int list_delete_by_id(LinkedList* L, int id);
//void list_sort(LinkedList* L, SortKey key, int ascending, const Student* st);

#endif

