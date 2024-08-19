#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql/mysql.h>
#include <sodium.h>

#define DB_HOST "localhost"
#define DB_USER "your_mysql_username"
#define DB_PASS "your_mysql_password"
#define DB_NAME "your_database_name"

// Function to initialize MySQL connection
MYSQL* init_db() {
    MYSQL *conn = mysql_init(NULL);
    if (!mysql_real_connect(conn, DB_HOST, DB_USER, DB_PASS, DB_NAME, 0, NULL, 0)) {
        fprintf(stderr, "Failed to connect to database: %s\n", mysql_error(conn));
        return NULL;
    }
    return conn;
}

// Function to check if the username already exists in the database
int username_exists(MYSQL *conn, const char *username) {
    char query[256];
    snprintf(query, sizeof(query), "SELECT username FROM users WHERE username = '%s'", username);
    if (mysql_query(conn, query)) {
        fprintf(stderr, "MySQL query error: %s\n", mysql_error(conn));
        return -1;  // Error in query
    }

    MYSQL_RES *result = mysql_store_result(conn);
    int num_rows = mysql_num_rows(result);
    mysql_free_result(result);

    return num_rows > 0;
}

// Function to insert a new user into the database
int insert_user(MYSQL *conn, const char *username, const char *hashed_password, const char *first_name, const char *last_name, const char *email) {
    char query[512];
    snprintf(query, sizeof(query), "INSERT INTO users (username, password, first_name, last_name, email) VALUES ('%s', '%s', '%s', '%s', '%s')",
             username, hashed_password, first_name, last_name, email);

    if (mysql_query(conn, query)) {
        fprintf(stderr, "MySQL insertion error: %s\n", mysql_error(conn));
        return 0;
    }

    return 1;
}

// Function to hash a password
void hash_password(const char *password, char *output) {
    unsigned char salt[crypto_pwhash_SALTBYTES];
    randombytes_buf(salt, sizeof salt); // Generate a random salt

    if (crypto_pwhash_str(output, password, strlen(password),
                          crypto_pwhash_OPSLIMIT_INTERACTIVE,
                          crypto_pwhash_MEMLIMIT_INTERACTIVE) != 0) {
        fprintf(stderr, "Out of memory while hashing the password\n");
        exit(1);
    }
}

int main() {
    if (sodium_init() < 0) {
        fprintf(stderr, "Failed to initialize libsodium\n");
        return 1;
    }

    char username[100], password[100], first_name[100], last_name[100], email[100], hashed_password[crypto_pwhash_STRBYTES];
    printf("Enter username: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = 0;

    printf("Enter password: ");
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = 0;

    printf("Enter first name: ");
    fgets(first_name, sizeof(first_name), stdin);
    first_name[strcspn(first_name, "\n")] = 0;

    printf("Enter last name: ");
    fgets(last_name, sizeof(last_name), stdin);
    last_name[strcspn(last_name, "\n")] = 0;

    printf("Enter email: ");
    fgets(email, sizeof(email), stdin);
    email[strcspn(email, "\n")] = 0;

    hash_password(password, hashed_password);

    MYSQL *conn = init_db();
    if (!conn) {
        return 1;
    }

    if (username_exists(conn, username) == 1) {
        printf("Username already exists.\n");
    } else if (username_exists(conn, username) == 0) {
        if (insert_user(conn, username, hashed_password, first_name, last_name, email)) {
            printf("Registration succeeded.\n");
        } else {
            printf("Registration failed.\n");
        }
    } else {
        printf("Error checking username.\n");
    }

    mysql_close(conn);
    return 0;
}
