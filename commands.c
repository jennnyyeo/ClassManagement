#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "commands.h"
#include "linked_list.h"

/*
 * Small helper to move a char pointer forward over spaces and tabs.
 * I use this when parsing command arguments like "ID = 1234567".
 */
static const char *skip_ws(const char *p)
{
    while (*p == ' ' || *p == '\t')
        ++p;
    return p;
}

/*
 * SHOWALL command:
 * - First checks if a file/database has been opened.
 * - Then prints a table header and walks through the linked list,
 *   printing every student record.
 */
void show_all_cmd(const LinkedList *list, int fileOpened)
{
    // Make sure user has opened a database file first
    if (!fileOpened) {
        printf("CMS: Please OPEN the database before inserting records.\n");
        return; // exit the function immediately
    }

    // If the list is empty or not initialised, just say there are no records
    if (!list || !list->head)
    {
        puts("(no records)");
        return;
    }

    size_t records = 0;

    // Print header row for the table
    printf("%-10s %-22s %-26s %-6s\n", "ID", "Name", "Programme", "Mark");
    puts("---------- ---------------------- -------------------------- ------");

    // Loop through each node and print the student info
    for (const Node *n = list->head; n; n = n->next)
    {
        printf("%-10d %-22.22s %-26.26s %6.2f\n",
               n->s.id, n->s.name, n->s.programme, n->s.mark);
        records++;
    }

    // Show total number of records at the end
    printf("There are in total %zu record(s).\n", records);
}

/*
 * INSERT command:
 * - Interactively asks the user for a new student's details.
 * - Validates each field (ID, Name, Programme, Mark).
 * - Ensures ID is unique.
 * - Adds the new record to the end of the linked list.
 */
void insertStudentRecords(LinkedList *list, int fileOpened) 
{
    // Must have an opened file before we allow insert
    if (!fileOpened) {
        printf("CMS: Please OPEN the database before inserting records.\n");
        return; // exit the function immediately
    }
    
    char buffer[22];
    Student s;

    /* ---------- Get and validate Student ID ---------- */
    while (1) {
        printf("Enter Student ID (7 digits): ");
        // Read user input into buffer
        if (!fgets(buffer, sizeof(buffer), stdin)) continue;  // in case of EOF

        // Remove trailing newline if present
        buffer[strcspn(buffer, "\n")] = '\0';

        // Check length must be exactly 7 digits
        if (strlen(buffer) != 7) {
            printf("CMS: Student ID must be exactly 7 digits.\n");
            continue;
        }

        // Check that every character is a digit
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

        // Now it is safe to convert to integer
        int id = atoi(buffer);
        
        // Check for duplicate ID in the existing linked list
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
            // Ask for a new ID again
            continue;
        }
            
        // Valid and unique ID, we can store it
        s.id = id;
        break;
    }

    /* ---------- Get and validate Student Name ---------- */
    while (1) {
        printf("Enter Student Name (max 22 characters): ");
        if (!fgets(s.name, MAX_NAME, stdin)) continue;
        s.name[strcspn(s.name, "\n")] = '\0'; // remove newline

        // Disallow empty name
        if (strlen(s.name) == 0) {
            printf("CMS: Student Name cannot be empty.\n");
            continue;
        }

        // Enforce max length 22 characters
        if (strlen(s.name) > 22) {
            printf("CMS: Student Name cannot exceed 22 characters.\n");
            continue;
        }
        
        // Name should only contain letters and spaces (no digits/punctuation)
        int valid = 1;
        for (int i = 0; s.name[i] != '\0'; i++) {
            if (!isalpha((unsigned char)s.name[i]) && s.name[i] != ' ') {
                valid = 0;
                break;
            }
        }
        
        if (!valid) {
            printf("CMS: Student Name must contain only letters.\n");
            continue;
        }
            
        // Reached here means name is valid
        break;
    }

    /* ---------- Get and validate Programme ---------- */
    while (1) {
        printf("Enter Programme: ");
        if (!fgets(s.programme, MAX_PROGRAM, stdin)) continue;
        s.programme[strcspn(s.programme, "\n")] = '\0'; // remove newline

        // Disallow empty programme
        if (strlen(s.programme) == 0) {
            printf("CMS: Programme cannot be empty.\n");
            continue;
        }
        
        // Programme should only contain letters and spaces
        int valid = 1;
        for (int i = 0; s.programme[i] != '\0'; i++) {
            if (!isalpha((unsigned char)s.programme[i]) && s.programme[i] != ' ') {
                valid = 0;
                break;
            }
        }
        
        if (!valid) {
            printf("CMS: Programme must contain only letters.\n");
            continue;
        }
            
        break; // valid programme entered
    }

    /* ---------- Get and validate Student Mark ---------- */
    while (1) {
        printf("Enter Student Mark: ");
        if (!fgets(buffer, sizeof(buffer), stdin)) continue;
        
        buffer[strcspn(buffer, "\n")] = '\0'; // remove newline
        
        float mark;
        // sscanf returns 1 if it successfully reads a float into mark
        if (sscanf(buffer, "%f", &mark) == 1) {
            // Check mark range [0,100]
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

    /* ---------- Insert new student into linked list ---------- */

    // Allocate memory for a new node
    Node *newNode = malloc(sizeof(Node));
    if (!newNode) {
        puts("Error: Memory allocation failed.");
        return;
    }
    newNode->s   = s;
    newNode->next = NULL;

    // If list is empty, new node becomes head,
    // otherwise append to the end of the list.
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

/*
 * Helper to parse an ID from a string like "ID=123" or "id = 123".
 * Returns 1 on success and writes the ID into *id.
 * Returns 0 if the format is wrong.
 */
static int parse_id(const char *args, int *id)
{
    const char *p = skip_ws(args);

    // Expect the keyword "ID=" (case-insensitive)
    if (!((p[0] == 'I' || p[0] == 'i') && (p[1] == 'D' || p[1] == 'd') && p[2] == '='))
    {
        return 0;
    }

    p += 3;                // move past "ID="
    p = skip_ws(p);        // skip any spaces after '='

    char *endp = NULL;
    long idv = strtol(p, &endp, 10);  // parse a long int from the string

    // If nothing was read, then the ID is missing or invalid
    if (endp == p)
        return 0;

    // After the number, we only allow whitespace and then end of string
    endp = (char *)skip_ws(endp);
    if (*endp != '\0')
        return 0;

    *id = (int)idv;
    return 1;
}

/*
 * Generic yes/no question reader:
 * - Reads a line from stdin.
 * - Accepts 'Y'/'y' as yes (returns 1).
 * - Accepts 'N'/'n' as no (returns 0).
 * - Keeps reprompting until user gives a valid answer.
 * - Treats EOF as 'No' (return 0).
 */
static int yes_no_qn(void)
{
    char buf[64];
    for (;;)
    {
        if (!fgets(buf, sizeof buf, stdin))
            return 0; // treat EOF as "No"

        // strip newline characters
        buf[strcspn(buf, "\r\n")] = '\0';

        // skip leading spaces
        char *p = buf;
        while (*p == ' ' || *p == '\t')
            ++p;

        if (*p == 'Y' || *p == 'y')
            return 1;
        if (*p == 'N' || *p == 'n')
            return 0;

        // If we reach here, the answer was invalid → reprompt
        printf("CMS: Please type \"Y\" to confirm or \"N\" to cancel.\n");
        printf("P1_1: ");
        fflush(stdout); // keep prompt on same line as user input
    }
}

/*
 * QUERY command:
 * - Parses "ID=<id>" from args.
 * - Looks up the node with that ID using list_find_by_id.
 * - If found, prints the record in the same table format as SHOWALL.
 */
void query(const LinkedList *list, const char *args) {
    if (!list) { 
        puts("(no list)"); 
        return; 
    }

    int id = 0;
    if (!parse_id(args, &id)) {
        puts("Usage: QUERY ID=<id>");
        return;
    }

    // list_find_by_id in my linked_list.h takes a (non-const) LinkedList*
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

/*
 * DELETE command:
 * - Parses "ID=<id>" from args.
 * - Confirms with user (Y/N) before deleting.
 * - Calls list_delete_by_id to remove the node from the list.
 */
void delete(LinkedList *list, const char *args)
{
    int id = 0;
    if (!parse_id(args, &id))
    {
        puts("Use DELETE ID=<id>");
        return;
    }

    // First check if record exists
    Node *n = list_find_by_id(list, id);
    if (!n)
    {
        printf("CMS: The record with ID=%d does not exist.\n", id);
        return;
    }
    
    // Ask for confirmation before actually deleting
    printf("CMS: Are you sure you want to delete record with ID=%d Please type \"Y\" to confirm or \"N\" to cancel.\n", id);
    printf("P1_1: ");
    fflush(stdout); // keep cursor on same line

    if (!yes_no_qn())
    {
        printf("CMS: The deletion is cancelled.\n");
        return;
    }

    // Actually remove from the list
    int removedcheck = list_delete_by_id(list, id);
    if (removedcheck)
    {
        printf("CMS: The record with ID=%d is successfully deleted.\n", id);
    }
    else
    {
        // This path is unlikely since we already checked, but it's a safe fallback
        printf("CMS: The record with ID=%d does not exist.\n", id);
    }
}

/*
 * UPDATE command:
 * - Parses "ID=<id>" from args.
 * - Finds the corresponding student node.
 * - Asks the user for new Name, Programme, and Mark.
 * - Pressing Enter without typing anything will leave that field unchanged.
 * - Validates each field similar to INSERT.
 */
void updateStudentRecord(LinkedList *list, const char *args)
{
    int id = 0;
    if (!parse_id(args, &id))
    {
        puts("CMS: Use UPDATE ID =<id>");
        return;
    }

    // Try to find the existing record
    Node *n = list_find_by_id(list, id);
    if (!n)
    {
        printf("CMS: The record with ID=%d does not exist.\n", id);
        return;
    }

    printf("CMS: Record with ID=%d found.\n", id);

    char buffer[128];
    int fieldUpdated = 0; // track if any field was actually changed

    /* ---------- Update name (optional) ---------- */
    while(1)
    {
        printf("Enter new Student Name (current: %s): ", n->s.name);
        if (!fgets(buffer, sizeof(buffer), stdin)) continue;
        buffer[strcspn(buffer, "\n")] = '\0';

        // If user just presses Enter, we don't change the name
        if (strlen(buffer) == 0)
        {
            break;
        }

        // Only allow letters and spaces in the name
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

        // Copy into the struct's name field safely
        strncpy(n->s.name, buffer, MAX_NAME);
        n->s.name[MAX_NAME -1] = '\0';
        fieldUpdated = 1;
        break;
    }

    /* ---------- Update programme (optional) ---------- */
    while (1)
    {
        printf("Enter new Programme (current: %s): ", n->s.programme);
        if (!fgets(buffer, sizeof(buffer), stdin)) continue;
        buffer[strcspn(buffer, "\n")] = '\0';
        
        // If user presses Enter, keep old programme
        if (strlen(buffer) == 0)
        {
            break;
        }

        // Only allow letters and spaces in programme name
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

        strncpy(n->s.programme, buffer, MAX_PROGRAM);
        n->s.programme[MAX_PROGRAM -1] = '\0';
        fieldUpdated = 1;
        break;
    }

    /* ---------- Update mark (optional) ---------- */
    while (1)
    {
        printf("Enter new Student Mark (current: %.2f): ", n->s.mark);
        if (!fgets(buffer, sizeof(buffer), stdin)) continue;
        buffer[strcspn(buffer, "\n")] = '\0';

        // Empty input → no change
        if (strlen(buffer) == 0)
        {
            break;
        }
        
        float mark;
        // Try to parse a float from the input
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

    /* ---------- Final message based on whether anything changed ---------- */
    if (fieldUpdated)
    {
        printf("CMS: The record with ID=%d is successfully updated. \n", id);
    }
    else
    {
        printf("CMS: No changes made to the record with ID=%d. \n", id);
    }
}
   
/*
 * Helper to swap the Student data between two nodes.
 * I use this in bubble sort so I don't have to relink the nodes.
 */
void swapStudents(Node *a, Node *b) {
    Student temp = a->s;
    a->s = b->s;
    b->s = temp;
}

/*
 * Bubble sort on the linked list:
 * - Can sort by ID or MARK (based on the 'field' string).
 * - 'ascending' flag controls ascending (1) or descending (0) order.
 * - Uses swapStudents to swap the Student data inside nodes.
 */
void bubbleSortLinkedList(LinkedList *list, const char *field, int ascending) {
    // If list is empty or has only one element, nothing to sort
    if (!list || !list->head || !list->head->next) return;

    int swapped; 
    Node *ptr; 
    Node *lptr = NULL; // last node pointer (end boundary for each pass)

    do { 
        swapped = 0; 
        ptr = list->head; 

        while (ptr->next != lptr) { 
            int cmp = 0;

            // Compare by ID
            if (strcmp(field, "ID") == 0) {
                cmp = ptr->s.id - ptr->next->s.id;
            } 
            // Compare by MARK
            else if (strcmp(field, "MARK") == 0) {
                if (ptr->s.mark > ptr->next->s.mark)      cmp = 1;
                else if (ptr->s.mark < ptr->next->s.mark) cmp = -1;
                else                                      cmp = 0;
            }

            // If we want descending order, just flip the sign
            if (!ascending) cmp = -cmp;

            // If cmp > 0, then the two nodes are in wrong order → swap them
            if (cmp > 0) {
                swapStudents(ptr, ptr->next);
                swapped = 1;
            }

            ptr = ptr->next;
        }
        
        // After each pass, the last element is in correct position
        lptr = ptr;

    } while (swapped);  // Keep looping while we are still swapping
}

/*
 * SHOWSUMMARY command:
 * - If the list is empty, prints zeros and N/A.
 * - Otherwise, goes through all students and calculates:
 *     - total number of students
 *     - average mark
 *     - highest mark (and the student's name)
 *     - lowest mark (and the student's name)
 */
void show_summary(const LinkedList *list)
{
    // Handle empty list case cleanly
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
    float highest_mark = -1.0;   // start below possible range
    float lowest_mark  = 101.0;  // start above possible range
    char highest_name[MAX_NAME] = "";
    char lowest_name[MAX_NAME]  = "";

    // Go through the whole list and update stats
    for (const Node *n = list->head; n; n = n->next)
    {
        total_students++;
        total_marks += n->s.mark;

        // Track highest mark and name
        if (n->s.mark > highest_mark)
        {
            highest_mark = n->s.mark;
            strncpy(highest_name, n->s.name, MAX_NAME - 1);
            highest_name[MAX_NAME - 1] = '\0';
        }

        // Track lowest mark and name
        if (n->s.mark < lowest_mark)
        {
            lowest_mark = n->s.mark;
            strncpy(lowest_name, n->s.name, MAX_NAME - 1);
            lowest_name[MAX_NAME - 1] = '\0';
        }
    }

    // Compute average safely (though total_students will never be 0 here)
    float average_mark = total_students > 0 ? total_marks / total_students : 0.0;

    // Print out the summary nicely
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
        // This branch is just extra defensive code
        printf("Highest mark: N/A\n");
        printf("Lowest mark: N/A\n");
    }
}
