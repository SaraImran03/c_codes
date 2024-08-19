#include <microhttpd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>
#include <mysql/mysql.h>

#define PORT 8888

// Database connection parameters
const char *db_host = "localhost";
const char *db_user = "your_username";
const char *db_password = "your_password";
const char *db_name = "user_database";

// Function to hash a password
void hash_password(const char* password, char* outputBuffer) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, password, strlen(password));
    SHA256_Final(hash, &sha256);
    int i;
    for(i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(outputBuffer + (i * 2), "%02x", hash[i]);
    }
    outputBuffer[64] = 0;
}

// Connect to the database
MYSQL* connect_database() {
    MYSQL *conn = mysql_init(NULL);
    if (!mysql_real_connect(conn, db_host, db_user, db_password, db_name, 0, NULL, 0)) {
        fprintf(stderr, "MySQL connection error: %s\n", mysql_error(conn));
        mysql_close(conn);
        return NULL;
    }
    return conn;
}

// Insert user into the database
int register_user(const char* username, const char* password) {
    MYSQL *conn = connect_database();
    if (conn == NULL) return 0;

    char hashed_password[65];
    hash_password(password, hashed_password);

    char query[512];
    snprintf(query, sizeof(query), "INSERT INTO users (username, password) VALUES ('%s', '%s')", username, hashed_password);

    if (mysql_query(conn, query)) {
        fprintf(stderr, "MySQL query error: %s\n", mysql_error(conn));
        mysql_close(conn);
        return 0;
    }

    mysql_close(conn);
    return 1;
}

// Handler for incoming HTTP requests
static int answer_to_connection(void *cls, struct MHD_Connection *connection,
                                const char *url, const char *method,
                                const char *version, const char *upload_data,
                                size_t *upload_data_size, void **con_cls) {
    if (strcmp(method, "POST") == 0 && strcmp(url, "/register") == 0) {
        if (*upload_data_size > 0) {
            // Parse data
            char *data = strdup(upload_data);
            const char *username = strtok(data, "&");
            const char *password = strtok(NULL, "&");

            if (username && password) {
                username = username + strlen("username="); // Skip "username="
                password = password + strlen("password="); // Skip "password="

                if (register_user(username, password)) {
                    free(data);
                    return send_response(connection, "User registered successfully", MHD_HTTP_OK);
                }
            }
            free(data);
        }
        return send_response(connection, "Failed to register user", MHD_HTTP_BAD_REQUEST);
    }
    return MHD_NO;
}

int main() {
    struct MHD_Daemon *daemon = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY, PORT, NULL, NULL,
                                                 &answer_to_connection, NULL, MHD_OPTION_END);
    if (daemon == NULL) {
        fprintf(stderr, "Failed to start the server.\n");
        return EXIT_FAILURE;
    }

    printf("Server running on port %d. Press enter to terminate.\n", PORT);
    getchar();

    MHD_stop_daemon(daemon);
    return EXIT_SUCCESS;
}
