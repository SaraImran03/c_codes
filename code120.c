#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char *chunk1, *chunk2;
    int size = 10;  // Size of each memory chunk

    // Allocate memory for both chunks
    chunk1 = (char *)malloc(size * sizeof(char));
    chunk2 = (char *)malloc(size * sizeof(char));

    if (chunk1 == NULL || chunk2 == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1; // Return with error if memory allocation fails
    }

    // Initialize the first chunk with some data
    for (int i = 0; i < size; i++) {
        chunk1[i] = 'a' + i;  // Fill chunk1 with letters from a to j
    }

    // Copy the contents of the first chunk to the second chunk
    memcpy(chunk2, chunk1, size * sizeof(char));

    // Print the contents of the second chunk
    printf("Contents of the second chunk:\n");
    for (int i = 0; i < size; i++) {
        printf("%c ", chunk2[i]);
    }
    printf("\n");

    // Free the allocated memory
    free(chunk1);
    free(chunk2);

    return 0;
}
