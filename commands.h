#ifndef COMMANDS_H
#define COMMANDS_H

#include "linked_list.h"

void show_all_cmd(const LinkedList* list);
void query(const LinkedList *list, const char *args);
void delete(LinkedList *list, const char *args);
#endif

