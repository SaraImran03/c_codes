#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>
#include <mysql/mysql.h>

// Database credentials and settings
const char *hostname = "localhost";
const char *username = "your_mysql_username";
const char *password = "your_mysql_password";
const char *database = "your_database_name";

// Hash a password using SHA-256
void hash_password(const char *input, char *output) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, input, strlen(input));
    SHA256_Final(hash, &sha256);
    
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(output + (i * 2), "%02x", hash[i]);
    }
    output[64] = '\0'; // Null-terminate the result
}

// Connect to the MySQL database
MYSQL *connect_db() {
    MYSQL *conn = mysql_init(NULL);
    if (!mysql_real_connect(conn, hostname, username, password, database, 0, NULL, 0)) {
        fprintf(stderr, "MySQL connection error: %s\n", mysql_error(conn));
        return NULL;
    }
    return conn;
}

// Insert user data into the database
int insert_user(const char *username, const char *hashed_password) {
    MYSQL *conn = connect_db();
    if (!conn) return 0;

    char query[256];
    snprintf(query, sizeof(query), "INSERT INTO users (username, password) VALUES ('%s', '%s')", username, hashed_password);

    if (mysql_query(conn, query)) {
        fprintf(stderr, "MySQL query error: %s\n", mysql_error(conn));
        mysql_close(conn);
        return 0;
    }

    mysql_close(conn);
    return 1;
}

int main() {
    char username[100];
    char password[100];
    char hashed_password[65];  // Buffer for the hashed password

    printf("Enter username: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = 0; // Remove the newline character

    printf("Enter password: ");
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = 0; // Remove the newline character

    hash_password(password, hashed_password);

    if (insert_user(username, hashed_password)) {
        printf("User registered successfully.\n");
    } else {
        printf("Failed to register user.\n");
    }

    return 0;
}
