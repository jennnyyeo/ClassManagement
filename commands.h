#ifndef COMMANDS_H
#define COMMANDS_H

#include "linked_list.h"

void show_all_cmd(const LinkedList* list);
void insert_cmd(const char* key, const char* value, LinkedList* list);

#endif

