#ifndef OPERATIONS_H
#define OPERATIONS_H

#include "linked_list.h"

int opendb(LinkedList* store, const char* filename, int fileOpened);

int savedb(LinkedList *store, const char *filename);

int autoSave(LinkedList *list, int fileOpened);

int recoverChanges(const char *dbFile, const char *autoSaveFile);

#endif