#include <stdio.h>
#include <string.h>
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

	puts("Commands: OPEN | SHOW ALL | INSERT | QUERY ID=<id> | UPDATE ID=<id> | DELETE ID=<id> | EXIT | SAVE | HELP");
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
				printf("File has already been opened");
			}
			if (opendb(&studentData, "P3_1-CMS.txt") == -1)
			{
				printf("Failed to open, please free up some memory and try again. \n");
				continue;
			}
			fileopened = 1;
		}
		else if (strcmp(command, "INSERT") == 0)
        {
            insertStudentRecords(&studentData);
        }
		else if (strcmp(command, "SHOW ALL") == 0)
		{
			show_all_cmd(&studentData);
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
		}
		else if (strcmp(command, "UPDATE") == 0)
		{
			puts("Please do: UPDATE ID=<id> instead");
		}
		else if (strncmp(command, "DELETE ", 7) == 0)
		{
			delete(&studentData, command + 7);
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
			}
		}
		else if (strcmp(command, "HELP") == 0)
		{
			printf("Commands: OPEN | SHOW ALL | INSERT | QUERY ID=<id> | UPDATE ID=<id> | DELETE ID=<id> | EXIT | SAVE | HELP\n");
		}
	}
}