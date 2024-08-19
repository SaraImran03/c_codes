#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>

int main() {
    int fd;
    const char *filename = "secret.txt";
    const char *data = "U2FsdGVkX1/R+WzJcxgvX/Iw==";
    struct stat fileStat;

    // Open the file for writing only; create it if it doesn't exist
    fd = open(filename, O_WRONLY | O_CREAT, 0644);
    if (fd < 0) {
        perror("Failed to open file");
        return 1; // Exit if file can't be opened
    }

    // Write the provided string to the file
    if (write(fd, data, strlen(data)) < 0) {
        perror("Failed to write to file");
        close(fd);
        return 1; // Exit on write error
    }

    // Close the file
    close(fd);

    // Change the permissions to 128 (octal 0200)
    if (chmod(filename, 0128) < 0) {
        perror("Failed to set file permissions");
        return 1;
    }

    // Get and print the file permissions
    if (stat(filename, &fileStat) < 0) {
        perror("Failed to get file permissions");
        return 1;
    }

    // Print the permissions in octal format
    printf("The permissions of the file are: 0o%o\n", fileStat.st_mode & 0777);

    return 0;
}
