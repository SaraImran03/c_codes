#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    // Array of example elements
    const char *elements[] = {"Apple", "Banana", "Cherry", "Date", "Elderberry"};
    int arraySize = sizeof(elements) / sizeof(elements[0]); // Calculate the number of elements in the array

    // Check if a command line argument is provided
    if (argc != 2) {
        printf("Usage: %s <index>\n", argv[0]);
        return 1; // Exit with a non-zero value to indicate error
    }

    // Convert the command line argument to an integer
    int index = atoi(argv[1]); // atoi converts a string to an integer

    // Check if the index is within the valid range of the array
    if (index < 0 || index >= arraySize) {
        printf("Invalid input\n"); // Print error message if index is out of bounds
    } else {
        // Print the element at the given index
        printf("Element at index %d: %s\n", index, elements[index]);
    }

    return 0; // Exit with zero to indicate successful execution
}
