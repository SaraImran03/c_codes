#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <username>\n", argv[0]);
        return 1; // Return error code if the username is not provided
    }

    char command[256];
    int status;

    // Prepare the command to run
    snprintf(command, sizeof(command), "id %s", argv[1]);

    // Execute the command
    status = system(command);

    if (status == -1) {
        perror("Failed to execute command");
        return 1;
    }

    // Print the exit status of the command
    if (WIFEXITED(status)) {
        printf("Exit status: %d\n", WEXITSTATUS(status));
    } else {
        printf("The command did not terminate properly\n");
    }

    return 0;
}
