#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    // Ensure that at least two arguments are provided (program name and one index)
    if (argc < 3) {
        printf("Usage: %s <index>\n", argv[0]);
        return 1; // Return with error if not enough arguments
    }

    // Convert the first argument after the program name to an integer to use as the index
    int index = atoi(argv[1]); // argv[1] is the index input by the user

    // Check if the index is valid (i.e., there is a corresponding argument at that position)
    if (index < 0 || index >= argc - 1) {
        printf("Invalid index. Please enter a valid index between 0 and %d.\n", argc - 2);
        return 1; // Return with error if index is out of bounds
    }

    // Print the corresponding command line argument
    // Note: +1 in argv to adjust because argv[0] is the program name
    printf("Argument at index %d: %s\n", index, argv[index + 1]);

    return 0; // Return successfully
}
