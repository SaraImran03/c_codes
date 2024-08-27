#include <stdio.h>
#include <string.h>
#include <stdbool.h>

// Mock function to simulate user authentication based on username, password, and session
bool auth_mod(const char *username, const char *password, const char *session) {
    // Example: hardcoded check for simplicity
    if (strcmp(username, "moderator") == 0 && strcmp(password, "mod1234") == 0 && strcmp(session, "session123") == 0) {
        return true;
    }
    return false;
}

// This function is called when /check_mod URL is visited
void check_mod(const char *url) {
    char username[100];
    char password[100];
    char session[100];

    // Simulate extraction of username, password, and session from URL
    // In real scenario, you should parse the URL to extract these values
    sscanf(url, "/check_mod?username=%99s&password=%99s&session=%99s", username, password, session);

    // Removing URL encoded '&' from strings
    username[strcspn(username, "&")] = '\0';
    password[strcspn(password, "&")] = '\0';
    session[strcspn(session, "&")] = '\0';

    // Call auth_mod with the extracted values
    bool isModerator = auth_mod(username, password, session);

    if (isModerator) {
        printf("User %s is a moderator.\n", username);
    } else {
        printf("User %s is not a moderator.\n", username);
    }
}

int main() {
    // Example usage
    check_mod("/check_mod?username=moderator&password=mod1234&session=session123");

    return 0;
}
