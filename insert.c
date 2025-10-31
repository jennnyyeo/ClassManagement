#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

// Function to open database (tab-delimited)
void openDatabase(char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error: Cannot open file %s\n", filename);
        return;
    }

    studentCount = 0;
    char line[200];

    // Skip the header line
    if (fgets(line, sizeof(line), file) == NULL) {
        printf("Error: File is empty\n");
        fclose(file);
        return;
    }

    // Read each student record
    while (fgets(line, sizeof(line), file) != NULL) {
        if (strlen(line) <= 1) continue; // skip empty lines

        char* token;

        // ID
        token = strtok(line, "\t");
        if (!token) continue;
        students[studentCount].id = atoi(token);

        // Name
        token = strtok(NULL, "\t");
        if (!token) continue;
        strcpy(students[studentCount].name, token);

        // Programme
        token = strtok(NULL, "\t");
        if (!token) continue;
        strcpy(students[studentCount].programme, token);

        // Mark
        token = strtok(NULL, "\t\n");
        if (!token) continue;
        students[studentCount].mark = atof(token);

        studentCount++;
        if (studentCount >= MAX_STUDENTS) break;
    }

    fclose(file);
    printf("Database opened successfully. %d records loaded.\n", studentCount);
}

// Function to display all records
void showAllRecords() {
    // Print table header
    printf("+------------+----------------+----------------------+-------+\n");
    printf("| ID         | Name           | Programme            | Mark  |\n");
    printf("+------------+----------------+----------------------+-------+\n");

    // Print each student
    for (int i = 0; i < studentCount; i++) {
        printf("| %-10d | %-14s | %-20s | %-5.2f |\n",
               students[i].id,
               students[i].name,
               students[i].programme,
               students[i].mark);
    }

    // Print table bottom
    printf("+------------+----------------+----------------------+-------+\n");
}


int main() {
    char filename[] = "P3_1-CMS.txt"; // Make sure this file exists in the same folder
    openDatabase(filename);

    printf("\nAll Records:\n");
    showAllRecords();

    return 0;
}
