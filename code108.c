#include <stdio.h>
#include <stdlib.h>

int main() {
    char username[256]; // Buffer for username
    char command[512]; // Buffer to hold the complete command
    FILE *output;
    char line[1024]; // Buffer to hold each line of output

    printf("Enter the username to fetch info for: ");
    scanf("%255s", username); // Read username with maximum of 255 characters

    // Prepare the command to fetch user details using getent
    snprintf(command, sizeof(command), "getent passwd %s", username);

    // Execute the command and open a pipe to read its output
    output = popen(command, "r");
    if (output == NULL) {
        perror("Failed to run command");
        exit(1); // Exit if the command didn't run
    }

    // Read and display the output from the command
    while (fgets(line, sizeof(line), output) != NULL) {
        printf("%s", line);
    }

    // Close the pipe
    if (pclose(output) == -1) {
        perror("Failed to close command stream");
        exit(1);
    }

    return 0;
}
