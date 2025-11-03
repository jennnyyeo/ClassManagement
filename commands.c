#include <stdio.h>
#include <string.h>
#include "commands.h"
#include "linked_list.h"

void show_all_cmd(const LinkedList *list) {
    if (!list || !list->head) {
        puts("(no records)");
        return;
    }
    size_t records = 0;
    printf("%-10s %-22s %-26s %-6s\n", "ID", "Name", "Programme", "Mark");
    puts("---------- ---------------------- -------------------------- ------");

    for (const Node *n = list->head; n; n = n->next) {
        printf("%-10d %-22.22s %-26.26s %6.2f\n",
               n->s.id, n->s.name, n->s.programme, n->s.mark);
        records++;
    }
    printf("There are in total %zu record(s).\n", records);
}
