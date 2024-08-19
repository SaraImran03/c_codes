#include <microhttpd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

#define PORT 8888

sqlite3 *db;

// Function to send HTTP responses with HTML content
int send_html_response(struct MHD_Connection *connection, const char *html, int status_code) {
    struct MHD_Response *response;
    int ret;

    response = MHD_create_response_from_buffer(strlen(html), (void *)html, MHD_RESPMEM_MUST_COPY);
    if (!response)
        return MHD_NO;

    MHD_add_response_header(response, "Content-Type", "text/html");
    ret = MHD_queue_response(connection, status_code, response);
    MHD_destroy_response(response);
    return ret;
}

// Handler for validating username and password
int validate_credentials(struct MHD_Connection *connection, const char *username, const char *password) {
    const char *sql = "SELECT password FROM users WHERE username=?";
    sqlite3_stmt *stmt;
    int rc;

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        return send_html_response(connection, "Internal Server Error", MHD_HTTP_INTERNAL_SERVER_ERROR);
    }

    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const char *db_password = (const char *)sqlite3_column_text(stmt, 0);
        sqlite3_finalize(stmt);
        if (strcmp(password, db_password) == 0) {
            return send_html_response(connection, "<html><head><meta http-equiv='refresh' content='0;url=/session'></head><body>Redirecting to session...</body></html>", MHD_HTTP_OK);
        } else {
            return send_html_response(connection, "<html><body>Incorrect username or password.</body></html>", MHD_HTTP_BAD_REQUEST);
        }
    } else {
        sqlite3_finalize(stmt);
        return send_html_response(connection, "<html><body>Incorrect username or password.</body></html>", MHD_HTTP_BAD_REQUEST);
    }
}

// Connection handler for HTTP requests
int answer_to_connection(void *cls, struct MHD_Connection *connection,
                         const char *url, const char *method,
                         const char *version, const char *upload_data,
                         size_t *upload_data_size, void **con_cls) {
    if (strcmp(method, "POST") == 0 && strcmp(url, "/login") == 0) {
        char *username = MHD_lookup_connection_value(connection, MHD_POSTDATA_KIND, "username");
        char *password = MHD_lookup_connection_value(connection, MHD_POSTDATA_KIND, "password");
        return validate_credentials(connection, username, password);
    }

    // Serve login form for GET request
    if (strcmp(url, "/") == 0 && strcmp(method, "GET") == 0) {
        const char *login_page = 
            "<html><body>"
            "<form action='/login' method='post'>"
            "Username: <input type='text' name='username'><br>"
            "Password: <input type='password' name='password'><br>"
            "<input type='submit' value='Login'>"
            "</form>"
            "</body></html>";
        return send_html_response(connection, login_page, MHD_HTTP_OK);
    }

    return MHD_NO;
}

int main() {
    struct MHD_Daemon *daemon;

    if (sqlite3_open("login.db", &db) != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }

    char *error_msg;
    if (sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY, username TEXT UNIQUE, password TEXT);",
                     NULL, NULL, &error_msg) != SQLITE_OK) {
        fprintf(stderr, "Failed to create table: %s\n", error_msg);
        sqlite3_free(error_msg);
        sqlite3_close(db);
        return 1;
    }

    daemon = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY, PORT, NULL, NULL,
                              &answer_to_connection, NULL, MHD_OPTION_END);
    if (daemon == NULL) {
        sqlite3_close(db);
        return 1;
    }

    printf("Server running on port %d. Press enter to terminate.\n", PORT);
    getchar();

    MHD_stop_daemon(daemon);
    sqlite3_close(db);
    return 0;
}
