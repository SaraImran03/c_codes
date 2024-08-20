#include <stdio.h>
#include <string.h>
#include <ctype.h>

int main(int argc, char *argv[]) {
    // Check if exactly one argument is provided (excluding the program name)
    if (argc != 2) {
        fprintf(stderr, "Error: Expected exactly one argument.\n");
        return 1;  // Exit with an error code
    }

    // Get the argument and store it in a variable
    char *input = argv[1];

    // Print the original argument
    printf("Original argument: '%s'\n", input);

    // Trim trailing whitespace
    int len = strlen(input);
    while (len > 0 && isspace((unsigned char)input[len - 1])) {
        len--;
    }
    input[len] = '\0';  // Null-terminate the modified string

    // Print the trimmed argument
    printf("Trimmed argument: '%s'\n", input);

    // Exit the program
    return 0;
}
