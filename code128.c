#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER_SIZE 1024

/**
 * Escapes HTML special characters in a string.
 * @param input The input string containing potential HTML characters.
 * @return A new string with HTML characters escaped. The caller must free this string.
 */
char* escape_html(const char* input) {
    size_t outputSize = 1; // Start with size for null-terminator
    const char *p = input;
    while (*p) {
        switch (*p) {
            case '<': outputSize += 4; break; // "&lt;"
            case '>': outputSize += 4; break; // "&gt;"
            case '&': outputSize += 5; break; // "&amp;"
            case '\"': outputSize += 6; break; // "&quot;"
            case '\'': outputSize += 6; break; // "&apos;"
            default: outputSize++; break;
        }
        p++;
    }

    char *output = malloc(outputSize);
    if (!output) return NULL;

    char *d = output;
    while (*input) {
        switch (*input) {
            case '<': strcpy(d, "&lt;");   d += 4; break;
            case '>': strcpy(d, "&gt;");   d += 4; break;
            case '&': strcpy(d, "&amp;");  d += 5; break;
            case '\"': strcpy(d, "&quot;"); d += 6; break;
            case '\'': strcpy(d, "&apos;"); d += 6; break;
            default: *d++ = *input; break;
        }
        input++;
    }
    *d = '\0';
    return output;
}

/**
 * Reads a username from the request stream, escapes HTML characters,
 * constructs a greeting message, and writes it to the response stream.
 */
void http_hello_page(FILE *request, FILE *response) {
    char username[BUFFER_SIZE];

    if (fgets(username, BUFFER_SIZE, request) == NULL) {
        fprintf(response, "Failed to read username.\n");
        return;
    }

    // Remove newline character if present
    username[strcspn(username, "\n")] = '\0';

    // Escape HTML characters in username
    char *escaped_username = escape_html(username);
    if (!escaped_username) {
        fprintf(response, "Failed to allocate memory for escaped username.\n");
        return;
    }

    fprintf(response, "Hello %s\n", escaped_username);

    free(escaped_username);
}

int main() {
    // Assuming stdin and stdout are used for simulation purposes
    printf("Enter your username: ");
    http_hello_page(stdin, stdout);
    return 0;
}
