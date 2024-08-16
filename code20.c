#include <stdio.h>
#include <stdlib.h> // For using the atoi function

// Define a constant for the offset
#define OFFSET 1000

int main(int argc, char *argv[]) {
    // Check if exactly one argument is provided (excluding program name)
    if (argc != 2) {
        printf("Usage: %s <number>\n", argv[0]);
        return 1; // Exit with error code if usage is incorrect
    }

    int input = atoi(argv[1]); // Convert the command line argument to an integer
    int result = input + OFFSET; // Add the OFFSET to the input value

    printf("The result is: %d\n", result); // Print the resulting value

    return 0; // Return 0 indicating successful termination
}
