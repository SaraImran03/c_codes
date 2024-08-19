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

    // Allocate the buffer on the heap
    char *buffer = (char *)malloc(buffer_size * sizeof(char));
    if (!buffer) {
        fprintf(stderr, "Memory allocation failed.\n");
        return EXIT_FAILURE;
    }

    // Initialize the buffer (example with zero)
    for (int i = 0; i < buffer_size; i++) {
        buffer[i] = 0; // Fill buffer with zeros
    }

    // Print the buffer contents
    printf("Buffer contents: ");
    for (int i = 0; i < buffer_size; i++) {
        printf("%02x ", buffer[i]);
    }
    printf("\n");

    // Free the allocated memory
    free(buffer);

    return EXIT_SUCCESS;
}
