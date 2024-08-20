#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Structure to represent the environment that contains the template
typedef struct {
    char* template;
} Environment;

// Function to render a response using the given environment and a name parameter
char* render_response_from_env(Environment *env, const char *name) {
    if (env == NULL || env->template == NULL || name == NULL) {
        fprintf(stderr, "Invalid input to render_response_from_env function.\n");
        return NULL;
    }

    // Calculate the necessary size for the response
    size_t neededSize = snprintf(NULL, 0, env->template, name) + 1;
    char *response = malloc(neededSize);
    if (!response) {
        fprintf(stderr, "Failed to allocate memory for response.\n");
        return NULL;
    }

    // Render the template with the user's name
    snprintf(response, neededSize, env->template, name);
    return response; // The caller needs to free this memory
}

// Function that simulates handling a "home" page request
char* home(Environment *env) {
    const char *name = "User";  // Simulate retrieving the name from a user request
    char *response = render_response_from_env(env, name);
    return response;  // The caller needs to manage the memory for the response
}

int main() {
    // Initialize the environment with a specific template
    Environment env = { .template = "Hello, %s! Welcome to our service." };

    // Simulate the home function to handle a request
    char *response = home(&env);
    if (response) {
        printf("%s\n", response);
        free(response); // Clean up the dynamically allocated response
    }

    return 0;
}
