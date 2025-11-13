#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "commands.h"
#include "linked_list.h"

// Help skip whitespace
static const char *skip_ws(const char *p)
{
    while (*p == ' ' || *p == '\t')
        ++p;
    return p;
}

void show_all_cmd(const LinkedList *list, int fileOpened)
{
    if (!fileOpened) {
        printf("CMS: Please OPEN the database before inserting records.\n");
        return; // exit the function immediately
    }

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

void insertStudentRecords(LinkedList *list, int fileOpened) 
{
    if (!fileOpened) {
        printf("CMS: Please OPEN the database before inserting records.\n");
        return; // exit the function immediately
    }
    
    char buffer[22];
    int valid_id;
    Student s;

    while (1) {
        printf("Enter Student ID (7 digits): ");
        if (!fgets(buffer, sizeof(buffer), stdin)) continue;  // in case of EOF

        buffer[strcspn(buffer, "\n")] = '\0'; // remove newline

        // Check length
        if (strlen(buffer) != 7) {
            printf("CMS: Student ID must be exactly 7 digits.\n");
            continue;
        }

        // Check all characters are digits
        int valid = 1;
        for (int i = 0; i < 7; i++) {
            if (!isdigit((unsigned char)buffer[i])) {
                valid = 0;
                break;
            }
        }

        if (!valid) {
            printf("CMS: Student ID must contain only digits.\n");
            continue;
        }

        // Convert to integer
        int id = atoi(buffer);
        
        // Check for duplicate in the linked list
        Node *n = list->head;
        int duplicate = 0;
        
        while (n) {
            if (n->s.id == id) {
                duplicate = 1;
                break;
            }
            n = n->next;
        }

        if (duplicate) {
            printf("CMS: Student record with ID=%d already exists.\n", id);
            continue; // ask for ID again
            }
            
            // Valid and unique ID
            s.id = id;
            break;
    }

    while (1) {
        printf("Enter Student Name (max 22 characters): ");
        if (!fgets(s.name, MAX_NAME, stdin)) continue;
        s.name[strcspn(s.name, "\n")] = '\0'; // remove newline

        // Check for empty input
        if (strlen(s.name) == 0) {
            printf("CMS: Student Name cannot be empty.\n");
            continue; // reprompt
        }

        // Check maximum length
        if (strlen(s.name) > 22) {
            printf("CMS: Student Name cannot exceed 22 characters.\n");
            continue; // reprompt
        }
        
        // Check that the name contains only letters
        int valid = 1;
        for (int i = 0; s.name[i] != '\0'; i++) {
            if (!isalpha((unsigned char)s.name[i]) && s.name[i] != ' ') {
                valid = 0;
                break;
            }
        }
        
        if (!valid) {
            printf("CMS: Student Name must contain only letters.\n");
            continue; // reprompt
        }
            
        break; // valid name entered
    }

    while (1) {
        printf("Enter Programme: ");
        if (!fgets(s.programme, MAX_PROGRAM, stdin)) continue;
        s.programme[strcspn(s.programme, "\n")] = '\0'; // remove newline

        // Check for empty input
        if (strlen(s.programme) == 0) {
            printf("CMS: Programme cannot be empty.\n");
            continue; // reprompt
            }
        
        // Check that the programme contains only letters
        int valid = 1;
        for (int i = 0; s.programme[i] != '\0'; i++) {
            if (!isalpha((unsigned char)s.programme[i]) && s.programme[i] != ' ') {
                valid = 0;
                break;
            }
        }
        
        if (!valid) {
            printf("CMS: Programme must contain only letters.\n");
            continue; // reprompt
        }
            
        break; // valid name entered
    }

    while (1) {
        printf("Enter Student Mark: ");
        if (!fgets(buffer, sizeof(buffer), stdin)) continue;
        
        buffer[strcspn(buffer, "\n")] = '\0'; // remove newline
        
        float mark;
        // sscanf returns 1 if it successfully read a float
        if (sscanf(buffer, "%f", &mark) == 1) {
            // check valid range
            if (mark < 0 || mark > 100) {
                printf("CMS: Mark must be between 0 and 100.\n");
                continue; // reprompt
            }

            s.mark = mark;
            
            break; // valid input, exit loop
            
        } else {
            printf("Error: Please enter a valid number.\n");
        }
    }

    // Insert into the linked list (create a new node)
    Node *newNode = malloc(sizeof(Node));
    if (!newNode) {
        puts("Error: Memory allocation failed.");
        return;
    }
    newNode->s = s;
    newNode->next = NULL;

    // Add to end of list
    if (!list->head) {
        list->head = newNode;
    } else {
        Node *curr = list->head;
        while (curr->next)
            curr = curr->next;
        curr->next = newNode;
    }

    printf("CMS: Student record with ID=%d successfully inserted.\n", s.id);

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

void updateStudentRecord(LinkedList *list, const char *args)
{
    int id = 0;
    if (!parse_id(args, &id))
    {
        puts("CMS: Use UPDATE ID =<id>");
        return;
    }
    Node *n = list_find_by_id(list, id);
    if (!n)
    {
        printf("CMS: The record with ID=%d does not exist.\n", id);
        return;
    }
    printf("CMS: Record with ID=%d found.\n", id);
    char buffer[128];
    int fieldUpdated = 0; // just to keep track if anything is modified

    while(1)
    {
        printf("Enter new Student Name (current: %s): ", n->s.name);
        if (!fgets(buffer, sizeof(buffer), stdin)) continue;
        buffer[strcspn(buffer, "\n")] = '\0';

        if (strlen(buffer) == 0) // this is if user pressed enter means no modification 
        {
            break;
        }

        int valid = 1;
        for (int i = 0; buffer[i] != '\0'; i++)
        {
            if (!isalpha((unsigned char)buffer[i]) && buffer[i] != ' ')
            {
                valid = 0;
                break;
            }
        }
    if (!valid)
        {
            printf("Error: Name must contain only letters and spaces.\n");
            continue;
        }
   // if (strlen(buffer) ==0)
      //  {
        //    printf("Error: Name cannot be empty. \n");
        //    continue;
      //  }
    strncpy(n->s.name, buffer, MAX_NAME);
    n->s.name[MAX_NAME -1] = '\0';
    fieldUpdated = 1;
    break;
    }
    while (1)
    {
        printf("Enter new Programme (current: %s): ", n->s.programme);
        if (!fgets(buffer, sizeof(buffer), stdin)) continue;
        buffer[strcspn(buffer, "\n")] = '\0';
        
        if (strlen(buffer) == 0) // if user press enter means no modficiation
        {
            break;
        }
        int valid = 1;
        for (int i = 0; buffer[i] != '\0'; i++)
        {
            if (!isalpha((unsigned char)buffer[i]) && buffer[i] != ' ')
            {
                valid = 0;
                break;
            }
        }
        if (!valid)
        {
            printf("Error: Programme must contain only letters and spaces. \n");
            continue;
        }
        //if (strlen(buffer) == 0)
        //{
         //   printf("Error: Programme cannot be empty. \n");
        //    continue;
      //  }
        strncpy(n->s.programme, buffer, MAX_PROGRAM);
        n->s.programme[MAX_PROGRAM -1] = '\0';
        fieldUpdated = 1;
        break;
    }
    while (1)
    {
        printf("Enter new Student Mark (current: %.2f): ", n->s.mark);
        if (!fgets(buffer, sizeof(buffer), stdin)) continue;
        buffer[strcspn(buffer, "\n")] = '\0';

        if (strlen(buffer) == 0)
        {
            break;
        }
        
        float mark;
        if (sscanf(buffer, "%f", &mark) == 1)
        {
            if (mark < 0.0 || mark > 100.0)
            {
                printf("Error: Mark must be between 0 and 100.\n");
                continue;
            }
            n->s.mark = mark;
            fieldUpdated = 1;
            break;
        }
        else
        {
            printf("Error: Please enter a valid numeric mark.\n");
        }
    }
    if (fieldUpdated)
    {
        printf("CMS: The record with ID=%d is successfully updated. \n", id);
    }
    else
    {
        printf("CMS: No changes made to the record with ID=%d. \n", id);
    }
}
   
// swap two students
void swapStudents(Node *a, Node *b) {
    Student temp = a -> s;
    a -> s = b -> s;
    b -> s = temp;
}

// sort linked list by ID or MARK
void bubbleSortLinkedList(LinkedList *list, const char *field, int ascending) {
    if (!list || !list -> head || !list -> head -> next) return;

    int swapped; 
    Node *ptr; 
    Node *lptr = NULL; 

    do { 
        swapped = 0; 
        ptr = list -> head; 

        while (ptr -> next != lptr) { 
            int cmp = 0;

            if (strcmp(field, "ID") == 0) {
                cmp = ptr -> s.id - ptr -> next -> s.id;
            } else if (strcmp(field, "MARK") == 0) {
                if (ptr -> s.mark > ptr -> next -> s.mark) cmp = 1;
                else if (ptr -> s.mark < ptr -> next -> s.mark) cmp = -1;
                else cmp = 0;
            }

            if (!ascending) cmp = -cmp;

            if (cmp > 0) {
                swapStudents(ptr, ptr -> next);
                swapped = 1;
            }

            ptr = ptr -> next;
        }
        
        lptr = ptr;
    } while (swapped);
}

// calculate and display summary statistics
void show_summary(const LinkedList *list)
{
    if (!list || !list->head)
    {
        printf("Total number of students: 0\n");
        printf("Average mark: 0.00\n");
        printf("Highest mark: N/A\n");
        printf("Lowest mark: N/A\n");
        return;
    }

    size_t total_students = 0;
    float total_marks = 0.0;
    float highest_mark = -1.0;
    float lowest_mark = 101.0;
    char highest_name[MAX_NAME] = "";
    char lowest_name[MAX_NAME] = "";

    // Calculate statistics
    for (const Node *n = list->head; n; n = n->next)
    {
        total_students++;
        total_marks += n->s.mark;

        // Check for highest mark
        if (n->s.mark > highest_mark)
        {
            highest_mark = n->s.mark;
            strncpy(highest_name, n->s.name, MAX_NAME - 1);
            highest_name[MAX_NAME - 1] = '\0';
        }

        // Check for lowest mark
        if (n->s.mark < lowest_mark)
        {
            lowest_mark = n->s.mark;
            strncpy(lowest_name, n->s.name, MAX_NAME - 1);
            lowest_name[MAX_NAME - 1] = '\0';
        }
    }

    // Calculate average
    float average_mark = total_students > 0 ? total_marks / total_students : 0.0;

    // Display summary
    printf("CMS: Summary Statistics\n");
    printf("Total number of students: %zu\n", total_students);
    printf("Average mark: %.2f\n", average_mark);
    
    if (total_students > 0)
    {
        printf("Highest mark: %.2f (%s)\n", highest_mark, highest_name);
        printf("Lowest mark: %.2f (%s)\n", lowest_mark, lowest_name);
    }
    else
    {
        printf("Highest mark: N/A\n");
        printf("Lowest mark: N/A\n");
    }
}
