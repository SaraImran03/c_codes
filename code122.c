#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    // Allocate memory for two chunks of size 10 bytes each
    char *chunk1 = malloc(10 * sizeof(char));  // First chunk
    char *chunk2 = malloc(10 * sizeof(char));  // Second chunk

    if (chunk1 == NULL || chunk2 == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        // Free already allocated memory before exiting if one allocation fails after another succeeded
        if (chunk1 != NULL) free(chunk1);
        if (chunk2 != NULL) free(chunk2);
        return 1;  // Exit with failure status
    }

    // Fill the first chunk with some data
    for (int i = 0; i < 10; i++) {
        chunk1[i] = 'a' + i;  // Initialize chunk1 with alphabet letters
    }

    // Copy the contents of the first chunk to the second chunk
    memcpy(chunk2, chunk1, 10 * sizeof(char));

    // Free the first chunk
    free(chunk1);
    chunk1 = NULL;  // Avoid dangling pointer

    // Free the second chunk
    free(chunk2);
    chunk2 = NULL;  // Avoid dangling pointer

    // Since there's no output required, just return success
    return 0;
}
