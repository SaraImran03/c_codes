#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql/mysql.h>
#include <sodium.h>

// Database connection parameters
const char *db_host = "localhost";
const char *db_user = "your_username";
const char *db_password = "your_password";
const char *db_name = "your_database";

// Function to initialize the database connection
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

// Function to hash a password with a random salt and store it
int store_user(const char *username, const char *password) {
    MYSQL *conn = init_db();
    if (!conn) {
        return 0;
    }

    // Generate a random salt and hash the password
    unsigned char salt[crypto_pwhash_SALTBYTES];
    randombytes_buf(salt, sizeof(salt));
    char hashed_password[crypto_pwhash_STRBYTES];

    if (crypto_pwhash_str(hashed_password, password, strlen(password),
                          crypto_pwhash_OPSLIMIT_INTERACTIVE,
                          crypto_pwhash_MEMLIMIT_INTERACTIVE) != 0) {
        fprintf(stderr, "Hashing failed, possibly out of memory.\n");
        mysql_close(conn);
        return 0;
    }

    // Store the username, hashed password, and salt in the database
    char query[1024];
    char *escaped_hashed_password = malloc(2 * strlen(hashed_password) + 1);
    mysql_real_escape_string(conn, escaped_hashed_password, hashed_password, strlen(hashed_password));

    snprintf(query, sizeof(query), "INSERT INTO users (username, password) VALUES ('%s', '%s')",
             username, escaped_hashed_password);

    if (mysql_query(conn, query)) {
        fprintf(stderr, "MySQL query error: %s\n", mysql_error(conn));
        free(escaped_hashed_password);
        mysql_close(conn);
        return 0;
    }

    free(escaped_hashed_password);
    mysql_close(conn);
    return 1;
}

int main() {
    if (sodium_init() < 0) {
        fprintf(stderr, "Libsodium could not be initialized\n");
        return 1;
    }

    char username[100];
    char password[100];

    printf("Enter username: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = 0; // Remove newline character

    printf("Enter password: ");
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = 0; // Remove newline character

    if (store_user(username, password)) {
        printf("User successfully registered.\n");
    } else {
        printf("Failed to register user.\n");
    }

    return 0;
}
