#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SAFE_DIR "/safe/"

// Validate the filename to ensure it's just a simple filename without path components
int isValidFilename(const char *filename) {
    // Check if the filename contains a path separator
    if (strchr(filename, '/') != NULL || strchr(filename, '\\') != NULL) {
        return 0;  // Invalid filename
    }
    return 1;  // Valid filename
}

// Function to read and display the file from the safe directory
int readFileFromSafeDir(const char *filename) {
    char filepath[1024];

    // Ensure the filepath buffer is large enough to hold the full path
    if (snprintf(filepath, sizeof(filepath), "%s%s", SAFE_DIR, filename) >= sizeof(filepath)) {
        fprintf(stderr, "Error: File path is too long.\n");
        return EXIT_FAILURE;
    }

    FILE *file = fopen(filepath, "r");
    if (!file) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    printf("Contents of %s:\n", filepath);
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), file)) {
        printf("%s", buffer);
    }

    fclose(file);
    return EXIT_SUCCESS;
}

int main() {
    char filename[256];

    printf("Enter the filename to read from the safe directory: ");
    if (scanf("%255s", filename) != 1) {
        fprintf(stderr, "Error reading input.\n");
        return EXIT_FAILURE;
    }

    if (!isValidFilename(filename)) {
        fprintf(stderr, "Invalid filename. Filenames must not include path separators.\n");
        return EXIT_FAILURE;
    }

    return readFileFromSafeDir(filename);
}
