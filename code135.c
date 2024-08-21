#include <stdio.h>
#include <string.h>

// Function to check if the given username and password are correct
int check_mod(const char* username, const char* password) {
    // Predefined valid username and password
    const char* validUsername = "admin";
    const char* validPassword = "pass123";

    // Check if the provided credentials match the valid ones
    if (strcmp(username, validUsername) == 0 && strcmp(password, validPassword) == 0) {
        return 1; // True: credentials are correct
    } else {
        return 0; // False: credentials are not correct
    }
}

int main() {
    // Example usage of the function
    const char* username = "admin";
    const char* password = "pass123";

    // Simulate user input and check credentials
    int result = check_mod(username, password);
    if (result == 1) {
        printf("Login successful.\n");
    } else {
        printf("Login failed.\n");
    }

    return 0;
}
