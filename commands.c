#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "commands.h"
#include "linked_list.h"


static const char *skip_ws(const char *p)
{
    while (*p == ' ' || *p == '\t')
        ++p;
    return p;
}


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


void insertStudentRecords(LinkedList *list, int fileOpened) 
{
    // Must have an opened file before we allow insert
    if (!fileOpened) {
        printf("CMS: Please OPEN the database before inserting records.\n");
        return; // exit the function immediately if database not opened
    }
    
    char buffer[22]; // temporary buffer to store user input
    Student s; // structure to store validated student data

    // -----------------------------
    // Input and validate Student ID
    // -----------------------------
    while (1) {
        printf("Enter Student ID (7 digits): ");
        // Read user input into buffer; continue if EOF
        if (!fgets(buffer, sizeof(buffer), stdin)) continue;  // in case of EOF

        // Remove trailing newline if present
        buffer[strcspn(buffer, "\n")] = '\0';

        // Check that the ID has exactly 7 characters
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
            continue; // reprompt
        }

        // Convert the validated string ID to an integer
        int id = atoi(buffer);
        
        // Check for duplicate ID in the linked list
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
            continue; // reprompt
        }
            
        // Valid and unique ID, store it in the Student structure
        s.id = id;
        break;
    }

    // -----------------------------
    // Input and validate Student Name
    // -----------------------------
    while (1) {
        printf("Enter Student Name (max 22 characters): ");
        if (!fgets(s.name, MAX_NAME, stdin)) continue;

        // Remove newline
        s.name[strcspn(s.name, "\n")] = '\0'; 

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
        
        // Name must only contain letters and spaces
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
            
        // name is valid
        break;
    }

    // -----------------------------
    // Input and validate Programme
    // -----------------------------
    while (1) {
        printf("Enter Programme: ");
        if (!fgets(s.programme, MAX_PROGRAM, stdin)) continue;
        
        // Remove newline
        s.programme[strcspn(s.programme, "\n")] = '\0'; 

        // Disallow empty programme
        if (strlen(s.programme) == 0) {
            printf("CMS: Programme cannot be empty.\n");
            continue;
        }
        
        // Programme must only contain letters and spaces
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
        
        // programme is valid
        break; 
    }

     // -----------------------------
    // Input and validate Student Mark
    // -----------------------------
    while (1) {
        printf("Enter Student Mark: ");
        if (!fgets(buffer, sizeof(buffer), stdin)) continue;

        // Remove newline
        buffer[strcspn(buffer, "\n")] = '\0'; 
        
        float mark;
        // sscanf returns 1 if it successfully reads a float
        if (sscanf(buffer, "%f", &mark) == 1) {
            // Ensure mark is in range 0–100
            if (mark < 0 || mark > 100) {
                printf("CMS: Mark must be between 0 and 100.\n");
                continue; // reprompt
            }
            
            // store valid mark
            s.mark = mark;
            break; 

        } else {
            printf("CMS: Please enter a valid number.\n");
        }
    }

    // -----------------------------
    // Create a new Node
    // -----------------------------
    Node *newNode = malloc(sizeof(Node)); // allocate memory
    if (!newNode) {
        puts("CMS: Memory allocation failed."); // check allocation
        return;
    }
    newNode->s   = s; // copy student data
    newNode->next = NULL; // end of list

    // -----------------------------
    // Insert Node into Linked List
    // -----------------------------
    if (!list->head) {
        list->head = newNode; // first node becomes head
    } else {
        Node *curr = list->head;
        while (curr->next)
            curr = curr->next; // traverse to end
        curr->next = newNode; // append new node
    }

    printf("CMS: Student record with ID=%d successfully inserted.\n", s.id);
}

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

// This function updates an existing student's record based on the ID provided.
void updateStudentRecord(LinkedList *list, const char *args) // this function looks for student using studentID and then based on this, updates the record
{
    int id = 0; // creates an integer variable id, its initialized to 0 but this variable is basically for storing the student ID parsed from args
    if (!parse_id(args, &id)) // calls parse_id to extract the ID from args, if this thing fail, it will prompt an error message and return
    {
        puts("CMS: Use UPDATE ID =<id>");
        return;
    }

   
    Node *n = list_find_by_id(list, id); // this basically checks if the studentID exist inside the linkedlist
    if (!n) // this checks if the node is NULL, means studentID doesnt not exist inside the linkedlist
    {
        printf("CMS: The record with ID=%d does not exist.\n", id); // this will be printed out if studentID doesnt exist 
        return;
    }

    printf("CMS: Record with ID=%d found.\n", id); // this will be printed if the n is not NULL, means student ID exists

    char buffer[128]; // temporary buffer to hold user input for each field
    int fieldUpdated = 0; // this is just for tracking if theres any field updated or not

    // optional update for name, since user can just press enter to skip updating name
    while(1) // this is an infinite loop, will break out of it when invalid or valid input is given
    {
        printf("Enter new Student Name (current: %s): ", n->s.name); // prompt user for new name, shows current name in parentheses
        if (!fgets(buffer, sizeof(buffer), stdin)) continue; // read user input into buffer, if fgets fails, we just reprompt
        buffer[strcspn(buffer, "\n")] = '\0';

        // If user just presses Enter, we don't change the name
        if (strlen(buffer) == 0) // checks is user just pressed enter
        {
            break; // break the loop, means no change to name
        }

        // Only allow letters and spaces in the name
        int valid = 1; // assume that input is valid
        for (int i = 0; buffer[i] != '\0'; i++) // this will loop through each character in the buffer
        {
            if (!isalpha((unsigned char)buffer[i]) && buffer[i] != ' ') // checks each character typed by user if its not a letter or space
            {
                valid = 0; // mark as invalid
                break; // break out of the loop early
            }
        }

        if (!valid) // if input is not valid
        {
            printf("Error: Name must contain only letters and spaces.\n"); // print error message
            continue; // will reprompt for name
        }

       
        strncpy(n->s.name, buffer, MAX_NAME); // copy the valid name into the student struct
        n->s.name[MAX_NAME -1] = '\0'; // this ensures null termination
        fieldUpdated = 1; // this indicates that tehre is changes in the name field 
        break; // this will break out of the loop since we have valid input
    }
    // optional field since user can just press enter to skip updating a programme
    while (1) // infinite loop to keep asking for programme until valid input or skip
    {
        printf("Enter new Programme (current: %s): ", n->s.programme); // this will prompt for user for new programme, this will still show the current programme inside the parenthesis
        if (!fgets(buffer, sizeof(buffer), stdin)) continue; // read user input into buffer, if fgets fails, we just reprompt
        buffer[strcspn(buffer, "\n")] = '\0'; // this will remove the trailing newline character from the input
        
       
        if (strlen(buffer) == 0) //checks if user just pressed enter means no update to the programme
        {
            break; // break the loop, means no change to programme
        }


        int valid = 1; // assume input is valid
        for (int i = 0; buffer[i] != '\0'; i++) // loop through each character in the buffer
        {
            if (!isalpha((unsigned char)buffer[i]) && buffer[i] != ' ') // checks if each character is not a letter or space
            {
                valid = 0; // this will mark the input as invalid
                break; // break out of the loop early
            }
        }

        if (!valid) // if innput is invalid
        {
            printf("Error: Programme must contain only letters and spaces. \n"); // this error message will be printed
            continue; // reprompt for programme
        }

        strncpy(n->s.programme, buffer, MAX_PROGRAM); // this will make a copy of the valid programme into the student struct
        n->s.programme[MAX_PROGRAM -1] = '\0'; // this ensures null termination
        fieldUpdated = 1; // indicates that there is changes in the programme field
        break;
    }

    // optional field since user can just press enter to skip updating a mark
    while (1)
    {
        printf("Enter new Student Mark (current: %.2f): ", n->s.mark); // prompt user for new mark, shows current mark in parentheses
        if (!fgets(buffer, sizeof(buffer), stdin)) continue; // read user input into buffer, if fgets fails, we just reprompt
        buffer[strcspn(buffer, "\n")] = '\0'; // this will remove the trailing newline character from the input

        
        if (strlen(buffer) == 0) // checks if user just pressed enter
        {
            break;
        }
        
        float mark; // temporary variable to hold parsed mark
        
        if (sscanf(buffer, "%f", &mark) == 1) // tries to parse a float from the buffer
        {
            if (mark < 0.0 || mark > 100.0) // ensures thatb the mark is within valid range
            {
                printf("Error: Mark must be between 0 and 100.\n"); // print error message
                continue; // reprompt for mark
            }
            n->s.mark = mark; // update the mark in the student struct
            fieldUpdated = 1; // indicates that there is changes in the mark field
            break; // break out of the loop since we have valid input
        }
        else
        {
            printf("Error: Please enter a valid numeric mark.\n"); // print error message for invalid input
        }
    }

    
    if (fieldUpdated) // if any field was updated
    {
        printf("CMS: The record with ID=%d is successfully updated. \n", id); // print success message
    }
    else    
    {
        printf("CMS: No changes made to the record with ID=%d. \n", id); // print message indicating no changes were made
    }
}
   
// Function to swap the data of two Student nodes
void swapStudents(Node *a, Node *b) {
    Student temp = a->s; // store data of node a temporarily
    a->s = b->s; // copy data of node b into node a
    b->s = temp; // copy temp (original a) into node b
}

// Function to sort a linked list of students by a given field (ID or MARK)
// ascending = 1 for ascending order, 0 for descending
void bubbleSortLinkedList(LinkedList *list, const char *field, int ascending) {
    // If list is empty or has only one element, nothing to sort
    if (!list || !list->head || !list->head->next) return;

    int swapped; // flag to check if a swap occurred in a pass
    Node *ptr; // pointer to traverse the list
    Node *lptr = NULL; // marks the last node boundary for each pass

    do { 
        swapped = 0; // reset swap flag at start of each pass
        ptr = list->head; // start from the head of the list

        // Traverse until the last sorted node
        while (ptr->next != lptr) { 
            int cmp = 0; // comparison result between two nodes

            // Compare by ID
            if (strcmp(field, "ID") == 0) {
                cmp = ptr->s.id - ptr->next->s.id; // positive if current > next
            } 
            // Compare by MARK
            else if (strcmp(field, "MARK") == 0) {
                if (ptr->s.mark > ptr->next->s.mark)      cmp = 1;
                else if (ptr->s.mark < ptr->next->s.mark) cmp = -1;
                else                                      cmp = 0;
            }

            // Flip comparison result if descending order is requested
            if (!ascending) cmp = -cmp;

            // Swap nodes if they are in the wrong order
            if (cmp > 0) {
                swapStudents(ptr, ptr->next); // swap the student data
                swapped = 1; // mark that a swap occurred
            }

            // move to the next node
            ptr = ptr->next;
        }
        
        // After each pass, the last node is in its correct position
        lptr = ptr;

    } while (swapped); // Repeat passes until no swaps are needed
}

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
