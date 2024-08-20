#include <stdio.h>
#include <string.h>
#include <ctype.h>

int main(int argc, char *argv[]) {
    // Check if the program received exactly one argument (excluding the program name)
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input-string>\n", argv[0]);
        return 1; // Exit with an error code if the number of arguments is incorrect
    }

    // Point to the first argument
    char *input = argv[1];

    // Print the original argument
    printf("Original argument: '%s'\n", input);

    // Trim trailing whitespace from the argument
    int len = strlen(input);
    while (len > 0 && isspace((unsigned char)input[len - 1])) {
        len--;
    }
    input[len] = '\0'; // Null-terminate the string after trimming

    // Print the trimmed argument
    printf("Trimmed argument: '%s'\n", input);

    return 0;
}
