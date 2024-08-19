#include <stdio.h>
#include <stdlib.h>

int main() {
    // Allocate 1024 bytes for the first buffer
    char *buf = (char *)malloc(1024 * sizeof(char));
    if (buf == NULL) {
        fprintf(stderr, "Failed to allocate memory for buffer\n");
        return EXIT_FAILURE;
    }

    // Allocate 1024 bytes for the second buffer
    char *buf1 = (char *)malloc(1024 * sizeof(char));
    if (buf1 == NULL) {
        fprintf(stderr, "Failed to allocate memory for buffer1\n");
        free(buf); // Clean up previously allocated buffer before exiting
        return EXIT_FAILURE;
    }

    // Prompt the user and read up to 100 characters into the first buffer
    printf("Enter up to 100 characters: ");
    if (fgets(buf, 100, stdin) == NULL) {
        fprintf(stderr, "Failed to read input into buffer\n");
    }

    // Prompt the user and read up to 100 characters into the second buffer
    printf("Enter up to 100 more characters: ");
    if (fgets(buf1, 100, stdin) == NULL) {
        fprintf(stderr, "Failed to read input into buffer1\n");
    }

    // Output the contents read into each buffer
    printf("You entered (buf): %s\n", buf);
    printf("You entered (buf1): %s\n", buf1);

    // Free the allocated memory
    free(buf);
    free(buf1);

    return EXIT_SUCCESS;
}
