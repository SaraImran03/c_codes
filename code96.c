#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql/mysql.h>
#include <sodium.h>

// Database connection details
const char *db_host = "localhost";
const char *db_user = "your_username";
const char *db_password = "your_password";
const char *db_name = "your_database";

// Initialize MySQL connection
MYSQL* init_db() {
    MYSQL *conn = mysql_init(NULL);
    if (conn == NULL) {
        fprintf(stderr, "MySQL initialization failed: %s\n", mysql_error(conn));
        return NULL;
    }

    if (mysql_real_connect(conn, db_host, db_user, db_password, db_name, 0, NULL, 0) == NULL) {
        fprintf(stderr, "MySQL connection failed: %s\n", mysql_error(conn));
        mysql_close(conn);
        return NULL;
    }

    return conn;
}

// Function to hash password and generate salt using libsodium
void hash_password(const char *password, char *hashed_password_storage) {
    unsigned char salt[crypto_pwhash_SALTBYTES];
    randombytes_buf(salt, sizeof(salt));  // Generate a random salt

    if (crypto_pwhash_str(hashed_password_storage, password, strlen(password),
                          crypto_pwhash_OPSLIMIT_INTERACTIVE, 
                          crypto_pwhash_MEMLIMIT_INTERACTIVE) != 0) {
        fprintf(stderr, "Out of memory while hashing password\n");
        exit(1);
    }
}

// Insert the user into the database with hashed password
int store_user(const char *username, const char *hashed_password) {
    MYSQL *conn = init_db();
    if (conn == NULL) return 0;

    char query[512];
    snprintf(query, sizeof(query), "INSERT INTO users (username, password) VALUES ('%s', '%s')",
             username, hashed_password);

    if (mysql_query(conn, query)) {
        fprintf(stderr, "Failed to insert user: %s\n", mysql_error(conn));
        mysql_close(conn);
        return 0;
    }

    mysql_close(conn);
    return 1;
}

int main() {
    if (sodium_init() < 0) {
        fprintf(stderr, "Libsodium initialization failed\n");
        return 1;
    }

    char username[100], password[100], hashed_password[crypto_pwhash_STRBYTES];

    printf("Enter username: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = 0; // Remove newline

    printf("Enter password: ");
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = 0; // Remove newline

    hash_password(password, hashed_password);

    if (store_user(username, hashed_password)) {
        printf("User registered successfully.\n");
    } else {
        printf("Failed to register user.\n");
    }

    return 0;
}
