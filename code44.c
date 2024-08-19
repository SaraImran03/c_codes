#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function to check if the filename is valid (contains no path specifiers)
int isValidFilename(const char *filename) {
    // Check for illegal characters in filename that can lead to path traversal
    return (strchr(filename, '/') == NULL && strchr(filename, '\\') == NULL);
}

// Main function that reads a file from the /safe/ directory
int main() {
    char filename[256];

    // Get filename from user input
    printf("Enter the filename to read from the /safe/ directory: ");
    if (scanf("%255s", filename) != 1) {
        printf("Failed to read input.\n");
        return EXIT_FAILURE;
    }

    // Validate the filename
    if (!isValidFilename(filename)) {
        printf("Invalid filename. Please ensure the filename does not contain path specifiers.\n");
        return EXIT_FAILURE;
    }

    // Construct the full path to the file in the /safe/ directory
    char filepath[300];
    snprintf(filepath, sizeof(filepath), "/safe/%s", filename);

    // Open the file
    FILE *file = fopen(filepath, "r");
    if (file == NULL) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    // Read and display the content of the file
    printf("Contents of %s:\n", filepath);
    char buffer[1024];  // Buffer to store file content
    while (fgets(buffer, sizeof(buffer), file)) {
        printf("%s", buffer);
    }

    // Close the file
    fclose(file);
    return EXIT_SUCCESS;
}
