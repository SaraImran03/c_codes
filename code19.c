#include <stdio.h>
#include <stdlib.h> // For atoi function

int main(int argc, char *argv[]) {
    // Check if the correct number of arguments is provided
    if (argc != 2) {
        printf("Usage: %s <number>\n", argv[0]);
        return 1; // Return an error code if the usage is incorrect
    }

    int input = atoi(argv[1]); // Convert the first argument to an integer
    int result = input + 1000; // Add 1000 to the input value

    printf("The result is: %d\n", result); // Output the result

    return 0; // Return 0 indicating successful termination
}
