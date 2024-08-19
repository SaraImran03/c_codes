#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define a structure to represent a person
typedef struct {
    char name[50];
    int status;
} person;

// Function to dynamically allocate memory for a person and initialize it
person* create_new_person() {
    // Allocate memory for one person struct
    person *p = (person *)malloc(sizeof(person));
    if (p == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE); // Exit if the memory allocation fails
    }

    // Initialize fields
    strcpy(p->name, "John"); // Initialize name to "John"
    p->status = 0;           // Set the status to 0

    return p;
}

int main() {
    // Allocate and initialize a new person
    person *myPerson = create_new_person();

    // Output the initialized data
    printf("Person's name: %s\n", myPerson->name);
    printf("Person's status: %d\n", myPerson->status);

    // Free the allocated memory
    free(myPerson);

    return 0;
}
