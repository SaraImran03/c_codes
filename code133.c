#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Structure to hold URL and the associated function pointer
typedef struct {
    const char* url;
    char* (*handler)(const char*);  // Function pointer to handle the request
} WebRoute;

// Handler function for a specific URL
char* handle_hello(const char* url) {
    static char response[256];
    snprintf(response, sizeof(response), "Hello, welcome to our site at %s", url);
    return response;
}

// Initialize the application with routes
WebRoute* create_app() {
    WebRoute* app = malloc(sizeof(WebRoute) * 1);  // Dynamic allocation for scalability in example
    if (app) {
        app[0].url = "/hello";
        app[0].handler = handle_hello;
    }
    return app;
}

// Simulate application server running
void run_application(WebRoute* app) {
    const char* test_url = "/hello";
    printf("Server running...\n");
    for (int i = 0; app[i].handler != NULL; i++) {
        if (strcmp(app[i].url, test_url) == 0) {
            char* response = app[i].handler(test_url);
            printf("Response: %s\n", response);
            break;
        }
    }
}

int main() {
    WebRoute* app = create_app();
    if (!app) {
        fprintf(stderr, "Failed to create the web application.\n");
        return 1;
    }
    run_application(app);
    free(app);  // Clean up allocated memory
    return 0;
}
