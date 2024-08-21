#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Mock function to simulate reading from a template file and inserting a username
char* render_template(const char *username) {
    const char *template = "Hello, %s! Welcome to our website."; // Simulating the content of hello.html
    int neededSize = snprintf(NULL, 0, template, username) + 1;
    char *response = malloc(neededSize);
    if (response) {
        snprintf(response, neededSize, template, username);
    }
    return response;  // The caller must free this memory
}

// Function to be "called" when a user visits /hello/<username>
char* hello(const char *username) {
    return render_template(username);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage: %s <username>\n", argv[0]);
        return 1;
    }

    char *username = argv[1];
    char *response = hello(username);
    if (response) {
        printf("%s\n", response);
        free(response);
    }

    return 0;
}
