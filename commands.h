#ifndef COMMANDS_H
#define COMMANDS_H

#include "linked_list.h"

void show_all_cmd(const LinkedList* list, int fileOpened);
void insertStudentRecords(LinkedList *list, int fileOpened);
void query(const LinkedList *list, const char *args);
void delete(LinkedList *list, const char *args);
void updateStudentRecord(LinkedList *list, const char * args);
void swapStudents(Node *a, Node *b);
void bubbleSortLinkedList(LinkedList *list, const char *field, int ascending);
void show_summary(const LinkedList *list);
#endif

