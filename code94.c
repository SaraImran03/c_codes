#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql/mysql.h>
#include <sodium.h>

#define DB_HOST "localhost"
#define DB_USER "your_db_user"
#define DB_PASS "your_db_password"
#define DB_NAME "your_db_name"

int connect_database(MYSQL **conn) {
    *conn = mysql_init(NULL);
    if (!*conn) {
        fprintf(stderr, "MySQL initialization failed\n");
        return 0;
    }

    if (!mysql_real_connect(*conn, DB_HOST, DB_USER, DB_PASS, DB_NAME, 0, NULL, 0)) {
        fprintf(stderr, "MySQL connection failed: %s\n", mysql_error(*conn));
        return 0;
    }

    return 1;
}

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

void hash_password(const char *password, char *output, size_t output_size) {
    unsigned char salt[crypto_pwhash_SALTBYTES];
    randombytes_buf(salt, sizeof salt);

    if (crypto_pwhash_str(output, password, strlen(password), 
                          crypto_pwhash_OPSLIMIT_INTERACTIVE, 
                          crypto_pwhash_MEMLIMIT_INTERACTIVE) != 0) {
        fprintf(stderr, "Out of memory\n");
        exit(1);
    }
}

int main() {
    if (sodium_init() < 0) {
        fprintf(stderr, "Cannot initialize libsodium\n");
        return 1;
    }

    char username[100], password[100], hashed_password[crypto_pwhash_STRBYTES];
    printf("Enter username: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = 0;

    printf("Enter password: ");
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = 0;

    hash_password(password, hashed_password, sizeof(hashed_password));

    MYSQL *conn;
    if (!connect_database(&conn)) {
        return 1;
    }

    if (insert_user(conn, username, hashed_password)) {
        printf("User registered successfully.\n");
    } else {
        printf("Failed to register user.\n");
    }

    mysql_close(conn);
    return 0;
}
