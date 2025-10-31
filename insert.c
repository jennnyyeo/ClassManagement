#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_NAME 50
#define MAX_PROGRAM 50
#define MAX_STUDENTS 100

typedef struct {
    int id;
    char name[MAX_NAME];
    char programme[MAX_PROGRAM];
    float mark;
} Student;

Student students[MAX_STUDENTS];
int studentCount = 0;

void openDatabase(char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error: Cannot open file %s\n", filename);
        return;
    }

    studentCount = 0;
    char line[200];

    while (fgets(line, sizeof(line), file) != NULL) {
        if (strlen(line) <= 1) continue; // skip empty lines

        int id;
        float mark;
        char nameAndProgram[150] = "";

        // Extract ID
        char* ptr = line;
        sscanf(ptr, "%d", &id);

        // Move ptr past ID
        while (*ptr != ' ' && *ptr != '\0') ptr++;
        while (*ptr == ' ') ptr++; // skip spaces

        // Extract mark from the end
        char* lastSpace = strrchr(ptr, ' ');
        if (!lastSpace) continue;
        mark = atof(lastSpace + 1); // last token = mark
        *lastSpace = '\0'; // terminate name + programme

        strcpy(nameAndProgram, ptr);

        // Split name and programme (assume first two words = name)
        char name[MAX_NAME] = "";
        char programme[MAX_PROGRAM] = "";

        char* space1 = strchr(nameAndProgram, ' ');
        if (space1 != NULL) {
            strncpy(name, nameAndProgram, space1 - nameAndProgram);
            name[space1 - nameAndProgram] = '\0';

            char* space2 = strchr(space1 + 1, ' ');
            if (space2 != NULL) {
                strncat(name, " ", 1);
                strncat(name, space1 + 1, space2 - space1 -1);
                strcpy(programme, space2 + 1);
            } else {
                strcat(name, " ");
                strcat(name, space1 +1);
                strcpy(programme, ""); // no programme?
            }
        } else {
            strcpy(name, nameAndProgram);
            strcpy(programme, "");
        }

        students[studentCount].id = id;
        strcpy(students[studentCount].name, name);
        strcpy(students[studentCount].programme, programme);
        students[studentCount].mark = mark;

        studentCount++;
        if (studentCount >= MAX_STUDENTS) break;
    }

    fclose(file);
    printf("Database opened successfully. %d records loaded.\n", studentCount);
}

void showAllRecords() {
    printf("ID\tName\tProgramme\tMark\n");
    printf("-------------------------------------------\n");
    for (int i = 0; i < studentCount; i++) {
        printf("%d\t%s\t%s\t%.2f\n",
               students[i].id,
               students[i].name,
               students[i].programme,
               students[i].mark);
    }
}

int main() {
    char filename[] = "P3_1-CMS.txt"; // Make sure this file exists in the same folder
    openDatabase(filename);

    printf("\nAll Records:\n");
    showAllRecords();

    return 0;
}
