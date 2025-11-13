#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "commands.h"
#include "operations.h"
#include "linked_list.h"

int main(void)
{

	LinkedList studentData;
	studentData.head = NULL;
	studentData.tail = NULL;

	char command[256];
	int fileopened = 0;

	if(recoverChanges("P3_1-CMS.txt", "autosave.txt"))
	{
		char choice[10];

		puts("CMS: There are changes to recover.");
		opendb(&studentData, "P3_1-CMS.txt", fileopened);
		fileopened = 1;
		puts("CMS: This is the current database state:");
		show_all_cmd(&studentData, fileopened);
		list_clear(&studentData);
		fileopened = 0;
		opendb(&studentData, "autosave.txt", fileopened);
		fileopened = 1;
		puts("\nCMS: This is the altered database:");
		show_all_cmd(&studentData, fileopened);
		printf("Would you like to recover the changes? (Y/N): ");

		while (1)
		{
			if (!fgets(choice, sizeof(choice), stdin)) continue;
				choice[strcspn(choice, "\n")] = '\0';
				// Convert to uppercase for consistency
				char c = toupper((unsigned char)choice[0]);
				if (c == 'Y' || c == 'N')
				{
					if (c == 'N') 
					{
						list_clear(&studentData);
						opendb(&studentData, "P3_1-CMS.txt", fileopened);
						autoSave(&studentData, fileopened);
						puts("CMS: Changes discarded.\n");
						break;
					}
					else if (c == 'Y')
					{
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

	puts("Commands: OPEN | SHOW ALL | INSERT | QUERY ID=<id> | UPDATE ID=<id> | DELETE ID=<id> | EXIT | SAVE | HELP");
	puts("Notes: Changes are automatically saved to 'autosave.txt' after each modification.");
	for (;;)
	{
		printf("Please input a command: ");
		if (!fgets(command, sizeof(command), stdin))
		{
			break;
		}

		// skip the trailing newline
		command[strcspn(command, "\n")] = '\0';

		// skip if empty
		if (command[0] == '\0')
		{
			puts("No command entered.");
			continue;
		}

		if (strcmp(command, "OPEN") == 0)
		{
			if (fileopened == 1)
			{
				printf("CMS: File has already been opened.\n");
			}
			if(fileopened == 0)
			{
				if (opendb(&studentData, "P3_1-CMS.txt", fileopened) == -1)
				{
					printf("Failed to open, please free up some memory and try again. \n");
					continue;
				}
			}
			fileopened = 1;
		}

		else if (strncmp(command, "SHOW ALL", 8) == 0) {
			if (!fileopened) {
				printf("CMS: Please OPEN the database before displaying records.\n");
				continue;
			}

			char choice[10];
			
			// Ask if user wants to sort
			while (1) {
				printf("Do you want to sort the records? (Y/N): ");

				if (!fgets(choice, sizeof(choice), stdin)) continue;
				choice[strcspn(choice, "\n")] = '\0';
				// Convert to uppercase for consistency
				char c = toupper((unsigned char)choice[0]);
				
				if (c == 'Y' || c == 'N') {
					if (c == 'N') {
						show_all_cmd(&studentData, fileopened);
						break; // skip sorting
					}
					
					// If 'Y', proceed to sorting
					while (1) {
						printf("Sort by ID or MARK? Enter ID or MARK: ");
						if (!fgets(choice, sizeof(choice), stdin)) continue;
						
						choice[strcspn(choice, "\n")] = '\0';
						// Convert to uppercase for consistency
						for (int i = 0; choice[i]; i++)
						choice[i] = toupper((unsigned char)choice[i]);

						if (strcmp(choice, "ID") == 0 || strcmp(choice, "MARK") == 0) {
							char order[10];
							int ascending = 1; // default
							
							while (1) {
								printf("Sort ascending or descending? (A/D): ");
								
								if (!fgets(order, sizeof(order), stdin)) continue;
								order[strcspn(order, "\n")] = '\0';
								// Convert to uppercase for consistency
								char o = toupper((unsigned char)order[0]);
								
								if (o == 'A') { ascending = 1; break; }
								else if (o == 'D') { ascending = 0; break; }
								else { printf("CMS: Please enter A or D.\n"); }
							}
							
							bubbleSortLinkedList(&studentData, choice, ascending);
							show_all_cmd(&studentData, fileopened);
							break; // done sorting
						} else {
							printf("CMS: Please enter either ID or MARK.\n");
						}
					}
					break; // done with SHOW ALL command
				} else {
					printf("CMS: Please enter Y or N.\n");
				}
			}
		}

		else if (strcmp(command, "INSERT") == 0)
        {
            insertStudentRecords(&studentData, fileopened);
			autoSave(&studentData, fileopened);
        }
		
		else if (strcmp(command, "EXIT") == 0)
		{
			break;
		}

		else if (strncmp(command, "QUERY ", 6) == 0)
		{
			query(&studentData, command + 6);
		}

		else if (strcmp(command, "QUERY") == 0)
		{
			puts("Please do: QUERY ID=<id> instead");
		}

		else if (strncmp(command, "UPDATE ", 7) == 0)
		{
			updateStudentRecord(&studentData, command + 7);
			autoSave(&studentData, fileopened);
		}

		else if (strcmp(command, "UPDATE") == 0)
		{
			puts("Please do: UPDATE ID=<id> instead");
		}

		else if (strncmp(command, "DELETE ", 7) == 0)
		{
			delete(&studentData, command + 7);
			autoSave(&studentData, fileopened);
		}

		else if (strcmp(command, "DELETE") == 0)
		{
			puts("Please do: DELETE ID=<id> instead");
		}

		else if (strcmp(command, "SAVE") == 0)
		{
			if (fileopened == 0)
			{
				puts("Please OPEN the file first");
			}
			else
			{
				if (savedb(&studentData, "P3_1-CMS.txt") == -1)
				{
					printf("Failed to open, please free up some memory and try again.\n");
					continue;
				}
				puts("File successfully saved.");
			}
		}

		else if (strcmp(command, "HELP") == 0)
		{
			printf("Commands: OPEN | SHOW ALL | INSERT | QUERY ID=<id> | UPDATE ID=<id> | DELETE ID=<id> | EXIT | SAVE | HELP\n");
		}
		else if (strcmp(command, "SUMMARY") == 0)  
        {
            show_summary(&studentData);
        }
	}
}
