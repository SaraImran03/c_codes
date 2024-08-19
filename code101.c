#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>

int main() {
    int fd;
    const char *filename = "important_config";
    const char *data = "important_config";

    // Open the file with O_WRONLY for writing only, O_CREAT to create it if it does not exist
    fd = open(filename, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd < 0) {
        perror("Failed to open file");
        return 1; // Return an error code
    }

    // Write the string "important_config" to the file
    if (write(fd, data, strlen(data)) != strlen(data)) {
        perror("Failed to write to file");
        close(fd);
        return 1;
    }

    printf("Successfully wrote to '%s'.\n", filename);

    // Close the file
    if (close(fd) < 0) {
        perror("Failed to close file");
        return 1;
    }

    return 0;
}
