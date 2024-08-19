#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql/mysql.h>
#include <bcrypt.h>

#define DB_HOST "localhost"
#define DB_USER "your_username"
#define DB_PASS "your_password"
#define DB_NAME "your_database"

// Initialize MySQL connection
MYSQL* init_db() {
    MYSQL *conn = mysql_init(NULL);
    if (!mysql_real_connect(conn, DB_HOST, DB_USER, DB_PASS, DB_NAME, 0, NULL, 0)) {
        fprintf(stderr, "Failed to connect to database: %s\n", mysql_error(conn));
        return NULL;
    }
    return conn;
}

// Function to hash a password
int hash_password(const char *password, char *hashed_password, size_t hashed_password_size) {
    if (bcrypt_hashpw(password, hashed_password, hashed_password_size) != 0) {
        fprintf(stderr, "Failed to hash password\n");
        return 0; // Hashing failed
    }
    return 1; // Hashing successful
}

// Insert the user into the database with hashed password
int store_user(const char *username, const char *hashed_password) {
    MYSQL *conn = init_db();
    if (!conn) return 0;

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
    char username[100], password[100], hashed_password[BCRYPT_HASHSIZE];

    printf("Enter username: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = 0; // Remove newline character

    printf("Enter password: ");
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = 0; // Remove newline character

    if (!hash_password(password, hashed_password, sizeof(hashed_password))) {
        printf("Password hashing failed.\n");
        return 1;
    }

    if (store_user(username, hashed_password)) {
        printf("User registered successfully.\n");
    } else {
        printf("Failed to register user.\n");
    }

    return 0;
}
