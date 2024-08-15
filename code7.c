#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    // Check if at least two arguments are provided (program name and index)
    if (argc < 3) {
        printf("Usage: %s <index_of_index> <other_arguments>\n", argv[0]);
        return 1; // Return 1 to indicate error in input
    }

    // Convert the second command line argument to an integer to use as the index
    int index_of_index = atoi(argv[1]); // This converts the string at argv[1] to an integer

    // Check if the calculated index is within the valid range of command line arguments
    if (index_of_index <= 0 || index_of_index >= argc) {
        printf("Invalid index provided. Please enter a valid index between 1 and %d.\n", argc - 1);
        return 1; // Return 1 to indicate error in index
    }

    // Retrieve the command line argument at the specified index
    int target_index = atoi(argv[index_of_index]); // Convert the argument at this index to an integer

    // Check if the target index is valid
    if (target_index < 0 || target_index >= argc) {
        printf("Target index out of range. Please ensure the target index is between 0 and %d.\n", argc - 1);
        return 1; // Return 1 to indicate error in target index
    }

    // Print the command line argument at the target index
    printf("Argument at index %d: %s\n", target_index, argv[target_index]);

    return 0; // Return 0 to indicate successful execution
}
