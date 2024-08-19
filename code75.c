#include <stdio.h>
#include <stdlib.h>

// Define a struct to represent a person
typedef struct {
    char name[100];
    int age;
    int status;  // Status field to be set to 0
} person;

// Function to dynamically allocate memory for a person and initialize it
person* allocate_new_person() {
    // Allocate memory for one person struct
    person *p = (person *)malloc(sizeof(person));  // Explicit cast is optional in C but included for clarity
    if (p == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);  // Exit if the memory allocation fails
    }

    // Initialize fields
    p->status = 0;  // Set the status to 0
    // Optionally initialize other fields here
    p->age = 0; // Example default initialization
    p->name[0] = '\0'; // Setting the name to an empty string

    return p;
}

int main() {
    // Allocate a new person
    person *new_person = allocate_new_person();

    // Example usage of the allocated person
    printf("New person's status: %d\n", new_person->status);
    printf("New person's age: %d\n", new_person->age);
    printf("New person's name: '%s'\n", new_person->name);

    // Free the allocated memory
    free(new_person);

    return 0;
}
