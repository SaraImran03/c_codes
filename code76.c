#include <stdio.h>
#include <stdlib.h>

// Define the person structure
typedef struct {
    char name[100];
    int age;
    int status; // Status field of the person
} person;

// Function to allocate memory for a person and initialize it
person* create_person() {
    // Dynamically allocate memory for one person struct
    person *p = (person *)malloc(sizeof(person));
    if (p == NULL) {
        fprintf(stderr, "Failed to allocate memory\n");
        exit(EXIT_FAILURE); // Exit if memory allocation fails
    }

    // Initialize fields
    p->status = 0;  // Initialize status to 0
    p->age = 0;     // Initialize age to a default value (0)
    p->name[0] = '\0'; // Initialize name to an empty string

    return p;
}

int main() {
    // Create a new person instance
    person *myPerson = create_person();

    // Access and print the person's status using the -> operator
    printf("Person's status: %d\n", myPerson->status);

    // Clean up: Free the allocated memory
    free(myPerson);

    return 0;
}
