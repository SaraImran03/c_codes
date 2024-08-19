#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function to execute a shell command and print its output
void execute_command(const char *cmd) {
    FILE *fp;
    char result[1024];  // Buffer to store the output of the command

    // Open the command for reading
    fp = popen(cmd, "r");
    if (fp == NULL) {
        printf("Failed to run command\n");
        exit(1);
    }

    // Read the output a line at a time and print it
    while (fgets(result, sizeof(result), fp) != NULL) {
        printf("%s", result);
    }

    // Close the file pointer to the process
    if (pclose(fp) < 0) {
        perror("Error closing stream.");
        exit(1);
    }
}

int main(int argc, char *argv[]) {
    char command[256];

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <username>\n", argv[0]);
        return 1;
    }

    // Prepare the command to fetch user details
    snprintf(command, sizeof(command), "cat /etc/passwd | grep '%s'", argv[1]);

    // Execute the command
    execute_command(command);

    return 0;
}
