#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>  // For mode constants

int main() {
    int fd;
    const char *filePath = "example_file";

    // Attempt to open the file with the specified flags and mode
    fd = open(filePath, O_CREAT | O_TRUNC | O_WRONLY | O_EXCL, S_IRUSR | S_IWUSR);
    if (fd < 0) {
        perror("Failed to open file");
        return 1; // Return an error if unable to open the file
    }

    printf("File '%s' opened successfully.\n", filePath);

    // Close the file descriptor
    if (close(fd) < 0) {
        perror("Failed to close file");
        return 1;
    }

    return 0;
}
