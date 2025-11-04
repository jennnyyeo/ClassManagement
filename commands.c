#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "commands.h"
#include "linked_list.h"

// Help skip whitespace
static const char *skip_ws(const char *p)
{
    while (*p == ' ' || *p == '\t')
        ++p;
    return p;
}

void show_all_cmd(const LinkedList *list)
{
    if (!list || !list->head)
    {
        puts("(no records)");
        return;
    }
    size_t records = 0;
    printf("%-10s %-22s %-26s %-6s\n", "ID", "Name", "Programme", "Mark");
    puts("---------- ---------------------- -------------------------- ------");

    for (const Node *n = list->head; n; n = n->next)
    {
        printf("%-10d %-22.22s %-26.26s %6.2f\n",
               n->s.id, n->s.name, n->s.programme, n->s.mark);
        records++;
    }
    printf("There are in total %zu record(s).\n", records);
}

static int parse_id(const char *args, int *id)
{
    const char *p = skip_ws(args);

    if (!((p[0] == 'I' || p[0] == 'i') && (p[1] == 'D' || p[1] == 'd') && p[2] == '='))
    {
        return 0;
    }
    p += 3;
    p = skip_ws(p);

    char *endp = NULL;
    long idv = strtol(p, &endp, 10);

    if (endp == p)
        return 0;

    endp = (char *)skip_ws(endp);
    if (*endp != '\0')
        return 0;

    *id = (int)idv;
    return 1;
}

static int yes_no_qn(void)
{
    char buf[64];
    for (;;)
    {
        if (!fgets(buf, sizeof buf, stdin))
            return 0; // treat EOF as "No"
        // strip newline
        buf[strcspn(buf, "\r\n")] = '\0';
        // skip leading spaces
        char *p = buf;
        while (*p == ' ' || *p == '\t')
            ++p;
        if (*p == 'Y' || *p == 'y')
            return 1;
        if (*p == 'N' || *p == 'n')
            return 0;
        // reprompt
        printf("CMS: Please type \"Y\" to confirm or \"N\" to cancel.\n");
        printf("P1_1: ");
        fflush(stdout); // User entering on the same line
    }
}

void query(const LinkedList *list, const char *args) {
    if (!list) { puts("(no list)"); return; }

    int id = 0;
    if (!parse_id(args, &id)) {
        puts("Usage: QUERY ID=<id>");
        return;
    }

    // list_find_by_id takes LinkedList* in your header; cast away const or
    // change its signature to take const LinkedList*.
    Node *n = list_find_by_id((LinkedList*)list, id);
    if (!n) {
        printf("No record with ID %d found.\n", id);
        return;
    }

    printf("%-10s %-22s %-26s %-6s\n", "ID", "Name", "Programme", "Mark");
    puts(  "---------- ---------------------- -------------------------- ------");
    printf("%-10d %-22.22s %-26.26s %6.2f\n",
           n->s.id, n->s.name, n->s.programme, n->s.mark);
}

void delete(LinkedList *list, const char *args)
{
    int id = 0;
    if (!parse_id(args, &id))
    {
        puts("Use DELETE ID=<id>");
        return;
    }

    Node *n = list_find_by_id(list, id);
    if (!n)
    {
        printf("CMS: The record with ID=%d does not exist.\n", id);
        return;
    }
    
    printf("CMS: Are you sure you want to delete record with ID=%d Please type \"Y\" to confirm or \"N\" to cancel.\n", id);
    printf("P1_1: ");
    fflush(stdout); // User entering on the same line

    if (!yes_no_qn())
    {
        printf("CMS: The deletion is cancelled.\n");
        return;
    }

    int removedcheck = list_delete_by_id(list, id);
    if (removedcheck)
    {
        printf("CMS: The record with ID=%d is successfully deleted.\n", id);
    }
    else
    {
        printf("CMS: The record with ID=%d does not exist.\n", id);
    }
}