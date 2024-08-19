#include <stdio.h>
#include <stdlib.h>

// Define a struct to represent a person
typedef struct {
    char name[100];
    int age;
} person;

// Function to allocate memory for a person struct and return a pointer to it
person* create_person() {
    // Dynamically allocate memory for one person struct
    person *p = malloc(sizeof(person));
    if (p == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    return p;
}

int main() {
    // Create a person struct instance dynamically
    person *p = create_person();

    // Use the -> operator to access struct members through a pointer
    strcpy(p->name, "John Doe");  // Setting the name of the person
    p->age = 30;                  // Setting the age of the person

    // Print the data
    printf("Name: %s\n", p->name);
    printf("Age: %d\n", p->age);

    // Free the allocated memory
    free(p);

    return 0;
}
