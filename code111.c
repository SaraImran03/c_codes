#include <stdio.h>
#include <stdlib.h>

// Function to simulate pinging a URL and returning the output
void ping(const char *url) {
    char command[256];
    FILE *fp;
    char result[1024];

    // Format the ping command with a limit of 4 echo requests
    snprintf(command, sizeof(command), "ping -c 4 %s", url);

    // Open the command for reading
    fp = popen(command, "r");
    if (fp == NULL) {
        perror("Failed to run command");
        exit(1);
    }

    // Read and display the output from the ping command
    printf("Output from ping command:\n");
    while (fgets(result, sizeof(result), fp) != NULL) {
        printf("%s", result);
    }

    // Close the pipe
    if (pclose(fp) < 0) {
        perror("Failed to close command stream");
        exit(1);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <URL>\n", argv[0]);
        return 1;
    }

    // Call the ping function with the URL provided from the command line
    ping(argv[1]);

    return 0;
}
