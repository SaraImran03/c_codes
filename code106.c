#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>

int main() {
    int fd;
    const char *filename = "super_secret.txt";
    const char *secretInfo = "Super Secret Info: Handle with care!";

    // Open the file for writing; create it if it doesn't exist
    fd = open(filename, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd < 0) {
        perror("Failed to open file");
        return 1; // Return an error code if unable to open the file
    }

    // Write the secret information to the file
    if (write(fd, secretInfo, strlen(secretInfo)) < 0) {
        perror("Failed to write to file");
        close(fd);
        return 1;
    }

    // Close the file descriptor
    if (close(fd) < 0) {
        perror("Failed to close file");
        return 1;
    }

    // Change file permissions to read-only for the owner
    if (chmod(filename, S_IRUSR) < 0) {
        perror("Failed to change file permissions");
        return 1;
    }

    // Print the name of the file
    printf("File created: %s\n", filename);

    return 0;
}
