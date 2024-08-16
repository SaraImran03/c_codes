#include <stdio.h>
#include <stdlib.h>  // For atoi function

int main(int argc, char *argv[]) {
    // Check if exactly one argument is provided (excluding program name)
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <number>\n", argv[0]);
        return 1; // Exit with error code if usage is incorrect
    }

    int input = atoi(argv[1]); // Convert the command line argument to an integer
    int result = input + 1000; // Add 1000 to the input value

    printf("The result is: %d\n", result); // Print the resulting value

    return 0; // Return 0 indicating successful termination
}
