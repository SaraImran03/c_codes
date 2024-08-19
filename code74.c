#include <stdio.h>
#include <stdlib.h>

// Define the person struct
typedef struct {
    char name[100];  // Name of the person
    int age;         // Age of the person
    int status;      // Additional field for status
} person;

// Function to allocate memory for a new person and initialize the status
person* create_new_person() {
    // Allocate memory for a new person
    person *p = (person *)malloc(sizeof(person)); // Casting the malloc return value to (person *)
    if (p == NULL) {
        fprintf(stderr, "Failed to allocate memory.\n");
        exit(EXIT_FAILURE); // Exit if the memory allocation fails
    }

    // Initialize the status field to 0
    p->status = 0;  // Using the -> operator to access fields of a struct through a pointer

    return p;
}

int main() {
    // Create a new person
    person *p = create_new_person();

    // Initialize other fields
    strcpy(p->name, "John Doe"); // Setting the name
    p->age = 25;                 // Setting the age

    // Print the details
    printf("Person's name: %s\n", p->name);
    printf("Person's age: %d\n", p->age);
    printf("Person's status: %d\n", p->status);

    // Free the allocated memory
    free(p);

    return 0;
}
