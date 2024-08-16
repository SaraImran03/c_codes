#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    // Check if there are enough arguments
    if (argc < 2) {
        printf("Usage: %s <numbers...>\n", argv[0]);
        return 1; // Exit the program with an error code
    }

    // Print the program name
    printf("Program name: %s\n", argv[0]);

    // Loop through each argument except the program name
    for (int i = 1; i < argc; i++) {
        // Convert the argument from a string to an integer
        int number = atoi(argv[i]);

        // Print the original string argument and its integer conversion
        printf("Argument %d as string: %s, converted to integer: %d\n", i, argv[i], number);
    }

    return 0; // Successful execution
}
