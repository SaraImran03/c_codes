#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <buffer size>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Parse the buffer size from the command line
    int buffer_size = atoi(argv[1]);
    if (buffer_size <= 0) {
        fprintf(stderr, "Please enter a valid buffer size.\n");
        return EXIT_FAILURE;
    }

    // Example loop where buffer is allocated and freed safely each iteration
    for (int i = 0; i < 5; i++) { // Limit the loop to 5 iterations for example
        printf("Iteration %d: Allocating and freeing a buffer of size %d\n", i + 1, buffer_size);

        // Allocate the buffer
        char *buffer = (char *)malloc(buffer_size * sizeof(char));
        if (!buffer) {
            fprintf(stderr, "Memory allocation failed.\n");
            return EXIT_FAILURE;
        }

        // Use the buffer (initialize it, for example)
        for (int j = 0; j < buffer_size; j++) {
            buffer[j] = (char)(j % 256); // Fill buffer with some data
        }

        // Free the buffer
        free(buffer);
    }

    return EXIT_SUCCESS;
}
