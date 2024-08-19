#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

int main() {
    int fd;
    const char *filename = "important_config";
    const char *data = "important_config";

    // Open the file with read and write access, create it if it does not exist
    fd = open(filename, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fd < 0) {
        perror("Failed to open file");
        return 1; // Return an error code if unable to open the file
    }

    // Write the string "important_config" to the file
    ssize_t bytes_written = write(fd, data, strlen(data));
    if (bytes_written < 0) {
        perror("Failed to write to file");
        close(fd);
        return 1;
    }

    printf("Successfully wrote '%s' to '%s'\n", data, filename);

    // Closing the file descriptor
    if (close(fd) < 0) {
        perror("Failed to close file");
        return 1;
    }

    return 0;
}
