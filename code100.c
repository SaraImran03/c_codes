#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>

int main() {
    int fileDescriptor;
    const char *filePath = "important_config";

    // Open the file with O_CREAT | O_TRUNC | O_WRONLY flags
    // Set permissions to S_IRUSR | S_IWUSR (read and write permissions for the owner)
    fileDescriptor = open(filePath, O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);

    if (fileDescriptor < 0) {
        perror("Failed to open file");
        return 1; // Return an error code
    }

    printf("File '%s' created successfully.\n", filePath);

    // Close the file descriptor
    if (close(fileDescriptor) < 0) {
        perror("Failed to close file");
        return 1;
    }

    return 0; // Success
}
