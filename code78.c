#include <stdio.h>
#include <stdlib.h>

// Define a struct to represent a person
typedef struct {
    char name[100];
    int age;
    int status; // Additional field for status
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
    return p;
}

int main() {
    // Create a new person instance
    person *myPerson = create_person();

    // Access and print the person's status using the -> operator
    printf("Person's status: %d\n", myPerson->status);

    // Free the allocated memory
    free(myPerson);

    return 0;
}
