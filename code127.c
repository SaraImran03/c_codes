#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Simulates processing an HTTP request and writing a response.
 * @param request FILE pointer representing the input stream containing the request.
 * @param response FILE pointer representing the output stream for the response.
 */
void http_hello_page(FILE *request, FILE *response) {
    char username[256];  // Buffer to store the username read from the request

    // Assume the username is the only content in the request stream
    if (fgets(username, sizeof(username), request) == NULL) {
        fprintf(response, "Failed to read username.\n");
        return;
    }

    // Remove any newline character at the end of username
    size_t len = strlen(username);
    if (len > 0 && username[len - 1] == '\n') {
        username[len - 1] = '\0';
    }

    // Construct and write the response
    fprintf(response, "Hello %s\n", username);
}

int main() {
    // Simulating the request and response streams
    // For demonstration purposes, using stdin and stdout
    printf("Please enter your username: ");
    fflush(stdout);  // Make sure 'Please enter your username:' is printed before input

    http_hello_page(stdin, stdout);

    return 0;
}
