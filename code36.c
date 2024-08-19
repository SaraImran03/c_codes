#include <microhttpd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sqlite3.h>

#define PORT 8888

sqlite3 *db;

// Send a response with the given status code and html content
int send_response(struct MHD_Connection *connection, const char *html, int status_code) {
    struct MHD_Response *response;
    int ret;

    response = MHD_create_response_from_buffer(strlen(html), (void *)html, MHD_RESPMEM_PERSISTENT);
    if (!response)
        return MHD_NO;

    MHD_add_response_header(response, "Content-Type", "text/html");
    ret = MHD_queue_response(connection, status_code, response);
    MHD_destroy_response(response);
    return ret;
}

// Handle login attempt
int login_function(struct MHD_Connection *connection, const char *username, const char *password) {
    char *sql = "SELECT password FROM users WHERE username=?";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const char *correct_password = (const char *)sqlite3_column_text(stmt, 0);
        if (strcmp(password, correct_password) == 0) {
            sqlite3_finalize(stmt);
            return send_response(connection, "<html><body>Login successful. Redirecting to home page...</body></html>", MHD_HTTP_OK);
        } else {
            sqlite3_finalize(stmt);
            return send_response(connection, "<html><body>Incorrect password.</body></html>", MHD_HTTP_UNAUTHORIZED);
        }
    } else {
        sqlite3_finalize(stmt);
        return send_response(connection, "<html><body>Username does not exist.</body></html>", MHD_HTTP_NOT_FOUND);
    }
}

// Connection handler
int answer_to_connection(void *cls, struct MHD_Connection *connection,
                         const char *url, const char *method,
                         const char *version, const char *upload_data,
                         size_t *upload_data_size, void **con_cls) {
    if (strcmp(method, "POST") == 0) {
        char *username = MHD_lookup_connection_value(connection, MHD_POSTDATA_KIND, "username");
        char *password = MHD_lookup_connection_value(connection, MHD_POSTDATA_KIND, "password");
        return login_function(connection, username, password);
    }

    // Serve a simple login form for GET requests
    if (strcmp(url, "/") == 0 && strcmp(method, "GET") == 0) {
        const char *page = "<html><body><form action='/' method='post'>"
                           "Username:<input type='text' name='username'><br>"
                           "Password:<input type='password' name='password'><br>"
                           "<input type='submit' value='Login'></form></body></html>";
        return send_response(connection, page, MHD_HTTP_OK);
    }

    return MHD_NO;
}

int main() {
    struct MHD_Daemon *daemon;
    int ret;

    sqlite3_open("login.db", &db);

    // Create the users table if it does not exist
    char *error;
    ret = sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY, username TEXT UNIQUE, password TEXT);", NULL, NULL, &error);
    if (ret != SQLITE_OK) {
        fprintf(stderr, "Failed to create table: %s\n", error);
        sqlite3_free(error);
        return 1;
    }

    daemon = MHD_start_daemon(MHD_USE_INTERNAL_POLLING_THREAD, PORT, NULL, NULL,
                              &answer_to_connection, NULL, MHD_OPTION_END);
    if (NULL == daemon) {
        sqlite3_close(db);
        return 1;
    }

    printf("Server running on port %d\n", PORT);
    getchar(); // Press Enter to stop the server

    MHD_stop_daemon(daemon);
    sqlite3_close(db);
    return 0;
}
