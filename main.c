#include <stdio.h>
#include <string.h>
#include "commands.h"
#include "operations.h"
#include "linked_list.h"

int main(void) {

	LinkedList studentData;
	studentData.head = NULL;
	studentData.tail = NULL;

	char command[256];
	int fileopened = 0;

	puts("Commands: OPEN <file> | SHOW ALL | EXIT");
	for (;;) {
		printf("Please input a command: ");
		if (!fgets(command, sizeof(command), stdin)){
			puts("\nGoodbye.");
			break;
		}

		// skip the trailing newline
		command[strcspn(command, "\n")] = '\0';

		//skip if empty
		if(command[0] == '\0'){
			continue;
		}

		if (strncmp(command, "OPEN ", 5) == 0) {
			if (fileopened == 1) {
				printf("File has already been opened");
			}
			else if (strcmp(command + 5, "P3_1-CMS.txt") == 0) {
				if (opendb(&studentData, command + 5) == -1){
					printf("Failed to open, please free up some memory and try again. \n");
					continue;
				}
				fileopened = 1;
			}
			else {
					printf("Invalid file name. Please key in again.\n");
			}
		}

		else if (strcmp(command, "SHOW ALL") == 0) {
			show_all_cmd(&studentData);
		}

		else if (strcmp(command, "EXIT") == 0){
			break;
		}

	}

}