#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>

// Function to hash a password using SHA-256
void hash_password(const char* password, char* outputBuffer) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, password, strlen(password));
    SHA256_Final(hash, &sha256);
    int i;
    for (i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(outputBuffer + (i * 2), "%02x", hash[i]);
    }
    outputBuffer[64] = '\0'; // Null-terminate the string
}

int main() {
    char username[100];
    char password[100];
    char hashed_password[65]; // SHA-256 outputs a 32-byte hash

    printf("Registration Form\n");
    printf("Enter username: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = 0; // Remove newline character

    printf("Enter password: ");
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = 0; // Remove newline character

    // Hash the password
    hash_password(password, hashed_password);

    // Display the results
    printf("You have registered with username: %s\n", username);
    printf("Your hashed password is: %s\n", hashed_password);

    return 0;
}
