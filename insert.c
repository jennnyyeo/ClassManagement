#include <stdio.h>
#include <string.h>
#include "linked_list.h"

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

// Function to display insert a record
void insertStudentRecords() 
{
    // Declare variables that will be used: id, name, program and mark
    int id;
    char name[MAX_NAME];
    char programme[MAX_PROGRAM];
    float mark;

    // Input user's information to be inserted into the database
    printf("Enter Student ID: ");
    // Save a decimal integer
    scanf("%d", &id);

    printf("Enter Student Name: ");
    // Save a string of characters
    scanf(" %s", name);

    printf("Enter Programme: ");
    // Save a string of characters
    scanf(" %s", programme);

    printf("Enter Student Mark: ");
    // Save a single-precision floating point number
    scanf("%f", &mark);

    // Primitive types (int, float, double, char) --> assign with =
    // Character arrays (char[]) --> use strcpy to copy strings
    students[studentCount].id = id;
    strcpy(students[studentCount].name, name);
    strcpy(students[studentCount].programme, programme);
    students[studentCount].mark = mark;

    studentCount++;

}

int main() {
    insertStudentRecords();
    
    return 0;
}

