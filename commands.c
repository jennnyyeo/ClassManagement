#include <stdio.h>
#include <string.h>
#include "commands.h"
#include "linked_list.h"

void show_all_cmd(LinkedList* list) {
	if (!list) {
		printf("(no list found)\n");
		return;
	}
	size_t records = 0;
	for (Node* n = list->head; n; n = n->next) {
		printf("%s %s\n", n->key ? n->key : "(null)", n->value ? n->value : "(null)");
		records++;
	}
	printf("There are in total of %zu records discovered.\n", records);
}
