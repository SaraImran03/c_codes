#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main() {
    int fd;
    const char *filePath = "example_file";
    const char *data = "important_config";
    ssize_t bytesWritten;

    // Open the file with read and write access, create it if it does not exist
    fd = open(filePath, O_RDWR | O_CREAT, 0700); // 0700 permissions mean read, write, and execute by owner only
    if (fd < 0) {
        perror("Failed to open file");
        return 1; // Return an error code if unable to open the file
    }

    // Write the string "important_config" to the file
    bytesWritten = write(fd, data, strlen(data));
    if (bytesWritten < 0) {
        perror("Failed to write to file");
        close(fd);
        return 1;
    }

    printf("Successfully wrote '%s' to '%s'.\n", data, filePath);

    // Closing the file descriptor
    if (close(fd) < 0) {
        perror("Failed to close file");
        return 1;
    }

    return 0;
}
