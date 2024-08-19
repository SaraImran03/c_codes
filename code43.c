#include <stdio.h>
#include <stdlib.h>

int main() {
    // Define the path to the file within the /safe/ directory
    const char *filePath = "/safe/example.txt";

    // Open the file in read mode
    FILE *file = fopen(filePath, "r");
    if (file == NULL) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    // Read and display the content of the file
    printf("Contents of %s:\n", filePath);
    char buffer[256];  // Buffer to store file content
    while (fgets(buffer, sizeof(buffer), file)) {
        printf("%s", buffer);
    }

    // Close the file
    fclose(file);
    return EXIT_SUCCESS;
}
