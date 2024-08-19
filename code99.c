#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql/mysql.h>
#include <bcrypt.h>

#define DB_HOST "localhost"
#define DB_USER "your_db_user"
#define DB_PASS "your_db_password"
#define DB_NAME "your_db_name"

// Function to initialize MySQL connection
MYSQL* init_db() {
    MYSQL *conn = mysql_init(NULL);
    if (conn == NULL) {
        fprintf(stderr, "MySQL initialization failed.\n");
        return NULL;
    }

    if (!mysql_real_connect(conn, DB_HOST, DB_USER, DB_PASS, DB_NAME, 0, NULL, 0)) {
        fprintf(stderr, "Failed to connect to database: %s\n", mysql_error(conn));
        mysql_close(conn);
        return NULL;
    }

    return conn;
}

// Function to hash a password
int hash_password(const char *password, char *hashed_password, size_t size) {
    if (bcrypt_hashpw(password, hashed_password, size) != 0) {
        fprintf(stderr, "Password hashing failed.\n");
        return 0; // Hashing failed
    }
    return 1; // Success
}

// Function to insert user into database
int insert_user(MYSQL *conn, const char *username, const char *hashed_password) {
    char query[512];
    snprintf(query, sizeof(query), "INSERT INTO users (username, password) VALUES ('%s', '%s')",
             username, hashed_password);

    if (mysql_query(conn, query)) {
        fprintf(stderr, "Failed to insert user: %s\n", mysql_error(conn));
        return 0;
    }

    return 1;
}

int main() {
    char username[100], password[100];
    char hashed_password[BCRYPT_HASHSIZE];

    printf("Enter username: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = 0; // Remove newline

    printf("Enter password: ");
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = 0; // Remove newline

    if (!hash_password(password, hashed_password, sizeof(hashed_password))) {
        printf("Failed to hash password.\n");
        return 1;
    }

    MYSQL *conn = init_db();
    if (conn == NULL) {
        return 1;
    }

    if (insert_user(conn, username, hashed_password)) {
        printf("User successfully registered.\n");
    } else {
        printf("Failed to register user.\n");
    }

    mysql_close(conn);
    return 0;
}
