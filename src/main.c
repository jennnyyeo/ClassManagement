#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "commands.h"
#include "operations.h"
#include "linked_list.h"

int main(void)
{
    // This linked list will store all the student records in memory
    LinkedList studentData;
    studentData.head = NULL;
    studentData.tail = NULL;

    char command[256];      // buffer to store user command input
    int fileopened = 0;     // flag to track whether the main DB file has been opened

    //Declaration
    printf("\t \t \t \t \t  Declaration\n");
    printf("SIT's policy on copying does not allow the students to copy source code as well as assessment solutions\nfrom another person AI or other places. It is the student's responsibility to guarantee that their\nassessment solutions are their own work. Meanwhile, the students must also ensure that their work is\nnot accessible by others. Where such plagiarism is detected, both of the assessments involved will\nreceive ZERO mark.\n\n");
    printf("We hereby declare that:\n\u2022  We fully understand and agree to the abovementioned plagiarism policy.\n\u2022  We did not copy any code from others or from other places.\n\u2022  We did not share our codes with others or upload to any other places for public access and will not do that in the future.\n\u2022  We agree that our project will receive Zero mark if there is any plagiarism detected.\n\u2022  We agree that we will not disclose any information or material of the group project to others or upload to any other places for public access.\n\u2022  We agree that we did not copy any code directly from AI generated sources.\n\n");
    printf("Declared by: P3-1\nTeam members:\n1.  Yeo Jessa Jenny Corrales\n2.  Wong Chiau Yee\n3.  Harith Dinie Bin Zainudin\n4.  Hoe Kai Jun\n5.  Joel Lee Jia Qing\n\n");
    printf("Date: 21/11/2025\n\n");

    //On startup, will compare current txt file with autosave, if different, will prompt user if want to recover
    if (recoverChanges("P3_1-CMS.txt", "autosave.txt"))
    {
        char choice[10];

        puts("CMS: There are changes to recover.");

        // First, open and show the original DB file so the user can see the baseline
        opendb(&studentData, "P3_1-CMS.txt", fileopened);
        fileopened = 1;
        puts("CMS: This is the current database state:");
        show_all_cmd(&studentData, fileopened);

        // Clear the list and then open the autosave version to show the "altered" state
        list_clear(&studentData);
        fileopened = 0;
        opendb(&studentData, "autosave.txt", fileopened);
        fileopened = 1;
        puts("\nCMS: This is the altered database:");
        show_all_cmd(&studentData, fileopened);

        // Ask user if they want to recover the autosaved changes
        printf("Would you like to recover the changes? (Y/N): ");

        while (1)
        {
            if (!fgets(choice, sizeof(choice), stdin)) 
                continue;

            choice[strcspn(choice, "\n")] = '\0';

            // Just look at the first character and normalise to uppercase
            char c = toupper((unsigned char)choice[0]);

            if (c == 'Y' || c == 'N')
            {
                if (c == 'N') 
                {
                    // User chose NOT to keep autosave → reload original DB and overwrite autosave
                    list_clear(&studentData);
                    opendb(&studentData, "P3_1-CMS.txt", fileopened);
                    autoSave(&studentData, fileopened);
                    puts("CMS: Changes discarded.\n");
                    break;
                }
                else if (c == 'Y')
                {
                    // User chose to recover autosave → save autosave content back to main DB file
                    savedb(&studentData, "P3_1-CMS.txt");
                    puts("CMS: Changes saved.\n");
                    break;
                }
            }
            else
            {
                puts("CMS: Please enter Y or N.");
            }	
        }
    }

    // Show basic help so the user knows what commands are available
    puts("Commands: OPEN | SHOW ALL | SUMMARY | INSERT | QUERY ID=<id> | UPDATE ID=<id> | DELETE ID=<id> | EXIT | SAVE | HELP");
    puts("Notes: Changes are automatically saved to 'autosave.txt' after each modification.");

    
    //It will prompt the user for a command, parses it and then dispatch to the corresponding function
    for (;;)
    {
        printf("Please input a command: ");
        if (!fgets(command, sizeof(command), stdin))
        {
            // On EOF or read error, just break out of the loop and exit
            break;
        }

        // Remove trailing newline from the command
        command[strcspn(command, "\n")] = '\0';

        // Handle empty input
        if (command[0] == '\0')
        {
            puts("No command entered.");
            continue;
        }

        /* ---------- OPEN ---------- */
        if (strcmp(command, "OPEN") == 0)
        {
            if (fileopened == 1)
            {
                printf("CMS: File has already been opened.\n");
            }
            if (fileopened == 0)
            {
                // Try to open the main database file
                if (opendb(&studentData, "P3_1-CMS.txt", fileopened) == -1)
                {
                    printf("Failed to open, please free up some memory and try again. \n");
                    continue;
                }
            }
            fileopened = 1;
        }

        //SHOW ALL (SORTING OPTIONAL)
        else if (strncmp(command, "SHOW ALL", 8) == 0) {
            // Check if database file has been opened
            if (!fileopened) {
                printf("CMS: Please OPEN the database before displaying records.\n");
                continue; // skip rest and prompt for a new command
            }

            // buffer to store user input
            char choice[10];
            
            // Ask user if they want to sort the records before displaying
            while (1) {
                printf("Do you want to sort the records? (Y/N): ");

                // Read input and handle EOF
                if (!fgets(choice, sizeof(choice), stdin)) continue;
                
                // Remove trailing newline
                choice[strcspn(choice, "\n")] = '\0';

                // Convert first character to uppercase for uniformity
                char c = toupper((unsigned char)choice[0]);
                
                // valid input
                if (c == 'Y' || c == 'N') {
                    if (c == 'N') {
                        // User chose not to sort, display list as-is
                        show_all_cmd(&studentData, fileopened);
                        break; // exit sorting loop
                    }
                    
                    // If user wants to sort, ask which field to sort by
                    while (1) {
                        printf("Sort by ID or MARK? Enter ID or MARK: ");
                        if (!fgets(choice, sizeof(choice), stdin)) continue;
                        
                        choice[strcspn(choice, "\n")] = '\0';

                         // Convert entire word to uppercase for comparison
                        for (int i = 0; choice[i]; i++)
                            choice[i] = toupper((unsigned char)choice[i]);

                        // Validate input: must be ID or MARK
                        if (strcmp(choice, "ID") == 0 || strcmp(choice, "MARK") == 0) {
                            char order[10];
                            int ascending = 1; // default ascending order
                            
                            // Ask user for sort order (ascending/descending)
                            while (1) {
                                printf("Sort ascending or descending? (A/D): ");
                                
                                if (!fgets(order, sizeof(order), stdin)) continue;
                                order[strcspn(order, "\n")] = '\0';

                                char o = toupper((unsigned char)order[0]);
                                
                                if (o == 'A') { 
                                    ascending = 1; 
                                    break; // valid order
                                }
                                else if (o == 'D') { 
                                    ascending = 0; 
                                    break; // valid order
                                }
                                else { 
                                    printf("CMS: Please enter A or D.\n"); 
                                }
                            }
                            
                            // Perform bubble sort on the linked list based on chosen field and order
                            bubbleSortLinkedList(&studentData, choice, ascending);
                            
                            // Display the sorted list
                            show_all_cmd(&studentData, fileopened);
                            break; // exit sort field loop
                        } else {
                            printf("CMS: Please enter either ID or MARK.\n"); // invalid field input
                        }
                    }
                    break; // exit main SHOW ALL loop
                } else {
                    printf("CMS: Please enter Y or N.\n");
                }
            }
        }

        //INSERT COMMAND
        else if (strcmp(command, "INSERT") == 0)
        {
            insertStudentRecords(&studentData, fileopened);
            // After modifying the list, auto-save to autosave.txt
            autoSave(&studentData, fileopened);
        }
		
        //EXIT COMMAND
        else if (strcmp(command, "EXIT") == 0)
        {
            // Just break out of the main loop and end the program
            break;
        }

        //QUERY COMMAND
        else if (strncmp(command, "QUERY ", 6) == 0)
        {
            // Pass arguments after "QUERY " to the query function
            query(&studentData, command + 6);
        }
        else if (strcmp(command, "QUERY") == 0)
        {
            // User forgot to specify ID, give a friendly hint
            puts("Please do: QUERY ID=<id> instead");
        }

        //UPDATE COMMAND
        else if (strncmp(command, "UPDATE ", 7) == 0)
        {
            updateStudentRecord(&studentData, command + 7);
            autoSave(&studentData, fileopened);
        }
        else if (strcmp(command, "UPDATE") == 0)
        {
            puts("Please do: UPDATE ID=<id> instead");
        }

        //DELETE COMMAND
        else if (strncmp(command, "DELETE ", 7) == 0)
        {
            delete(&studentData, command + 7);
            autoSave(&studentData, fileopened);
        }
        else if (strcmp(command, "DELETE") == 0)
        {
            puts("Please do: DELETE ID=<id> instead");
        }

        //SAVE COMMAND
        else if (strcmp(command, "SAVE") == 0)
        {
            if (fileopened == 0)
            {
                puts("Please OPEN the file first");
            }
            else
            {
                // Save the in-memory list back to the main DB file
                if (savedb(&studentData, "P3_1-CMS.txt") == -1)
                {
                    printf("Failed to open, please free up some memory and try again.\n");
                    continue;
                }
                puts("File successfully saved.");
            }
        }

        //HELP COMMAND
        else if (strcmp(command, "HELP") == 0)
        {
            // Re-print the list of available commands
            puts("Commands: OPEN | SHOW ALL | SUMMARY | INSERT | QUERY ID=<id> | UPDATE ID=<id> | DELETE ID=<id> | EXIT | SAVE | HELP");
        }

        //SUMMARY
        else if (strcmp(command, "SUMMARY") == 0)  
        {
            if (!fileopened)
            {
                puts("CMS: Please OPEN the database before displaying summary.");
                continue;
            }
            else
            {
                show_summary(&studentData);
            }
        }

        //IF COMMAND IS UNKNOWN
        else
        {
            puts("CMS: Unknown command. Type HELP for a list of commands.");
        }
    }
}
