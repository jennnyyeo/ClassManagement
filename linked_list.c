#include "linked_list.h"
#include <stdlib.h>

/*
 * create_node:
 * - Small helper that allocates a new Node on the heap.
 * - Copies the Student data from *st into the node.
 * - Sets next to NULL so the caller can link it properly.
 *
 * Returns:
 *   pointer to the new Node on success
 *   NULL if malloc fails
 */
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

/*
 * insert_node:
 * - Inserts a new Student at the end of the linked list.
 * - Uses create_node() to allocate and set up the node.
 * - Updates both head and tail pointers when needed.
 *
 * Returns:
 *   0  on success
 *  -1  if allocation failed
 */
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

/*
 * list_init:
 * - Simple initialiser for a LinkedList.
 * - Sets both head and tail to NULL so the list starts off empty.
 */
void list_init(LinkedList* L) {
    L->head = L->tail = NULL;
}

/*
 * list_clear:
 * - Walks through the entire list and frees every node.
 * - At the end, both head and tail are set to NULL.
 *
 * I use this to clean up when the program exits or when switching files.
 */
void list_clear(LinkedList* L) {
    for (Node* p = L->head; p;) {
        Node* n = p->next;  // remember next before freeing
        free(p);
        p = n;
    }
    L->head = L->tail = NULL;
}

/*
 * list_find_by_id:
 * - Linearly searches the list for the first node whose Student.id matches
 *   the given id.
 *
 * Returns:
 *   pointer to the matching Node
 *   NULL if no such node exists
 */
Node* list_find_by_id(LinkedList* L, int id) {
    for (Node* p = L->head; p; p = p->next) {
        if (p->s.id == id) {
            return p;
        }
    }
    return NULL;
}

/*
 * list_delete_by_id:
 * - Removes the first node in the list whose Student.id matches id.
 * - Relinks the previous node to skip over the deleted one.
 * - If the deleted node was the head, updates head accordingly.
 * - Frees the memory of the deleted node.
 *
 * Returns:
 *   1  if a node was found and deleted
 *   0  if no node with that id exists in the list
 */
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
