#include <stdio.h>
#include <stdlib.h>

int main() {
    char *buf;  // Pointer to store the allocated buffer

    // Allocate a buffer of 1024 bytes
    buf = (char *)malloc(1024);
    if (buf == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return EXIT_FAILURE; // Exit if memory allocation fails
    }

    printf("Enter a string (up to 10 characters will be read): ");
    
    // Read up to 10 characters from standard input
    if (fgets(buf, 11, stdin) == NULL) {
        fprintf(stderr, "Failed to read input\n");
        free(buf); // Free the allocated memory on error
        return EXIT_FAILURE;
    }

    // Print the string that was read
    printf("You entered: %s", buf);

    // Free the allocated memory
    free(buf);

    return 0;
}
