#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    const char* url;
    char* (*handler)(const char* username);
} Route;

char* escape(const char* input) {
    // This function assumes input is null-terminated and escapes characters to HTML-safe sequences.
    int length = strlen(input);
    int outLength = 0;
    char* output = malloc(length * 6 + 1); // Worst case each character turns into a 6-character sequence
    if (!output) return NULL;

    for (int i = 0; i < length; i++) {
        switch (input[i]) {
            case '<': strcat(output + outLength, "&lt;"); outLength += 4; break;
            case '>': strcat(output + outLength, "&gt;"); outLength += 4; break;
            case '&': strcat(output + outLength, "&amp;"); outLength += 5; break;
            case '\"': strcat(output + outLength, "&quot;"); outLength += 6; break;
            case '\'': strcat(output + outLength, "&apos;"); outLength += 6; break;
            default: output[outLength++] = input[i]; output[outLength] = '\0'; break;
        }
    }
    return output;
}

char* hello(const char* username) {
    char* safeUsername = escape(username);
    if (!safeUsername) return NULL;

    char* message = malloc(strlen("Hello, ") + strlen(safeUsername) + 2);
    if (!message) {
        free(safeUsername);
        return NULL;
    }

    sprintf(message, "Hello, %s!", safeUsername);
    free(safeUsername);
    return message;
}

void web_application(Route* routes, int numRoutes, const char* url) {
    for (int i = 0; i < numRoutes; i++) {
        if (strcmp(routes[i].url, url) == 0) {
            char* response = routes[i].handler("JohnDoe");
            if (response) {
                printf("Response: %s\n", response);
                free(response);
            } else {
                printf("Error handling request.\n");
            }
            return;
        }
    }
    printf("URL not found.\n");
}

int main() {
    Route routes[] = {
        {"/hello/username", hello}
    };
    int numRoutes = sizeof(routes) / sizeof(Route);

    web_application(routes, numRoutes, "/hello/username");

    return 0;
}
