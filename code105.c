#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>

int main() {
    int fd;
    const char *filename = "secret.txt";
    const char *data = "U2FsdGVkX1/R+WzJcxgvX/Iw==";
    ssize_t bytesWritten;

    // Open the file for writing only; create it if it doesn't exist
    fd = open(filename, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd < 0) {
        perror("Failed to open file");
        return 1; // Return an error code if unable to open the file
    }

    // Write the specified string to the file
    bytesWritten = write(fd, data, strlen(data));
    if (bytesWritten < 0) {
        perror("Failed to write to file");
        close(fd);
        return 1;
    }

    // Close the file to ensure all data is flushed to disk before changing permissions
    if (close(fd) < 0) {
        perror("Failed to close file");
        return 1;
    }

    // Change file permissions to be readable and writable only by the owner
    if (chmod(filename, S_IRUSR | S_IWUSR) < 0) {
        perror("Failed to change file permissions");
        return 1;
    }

    printf("File '%s' created and permissions set.\n", filename);

    return 0;
}
