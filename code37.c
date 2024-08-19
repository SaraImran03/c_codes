#include <microhttpd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sqlite3.h>

#define PORT 8888

sqlite3 *db;

// Function to send redirection responses
int send_redirect(struct MHD_Connection *connection, const char *location) {
    struct MHD_Response *response;
    int ret;

    response = MHD_create_response_from_buffer(0, "", MHD_RESPMEM_PERSISTENT);
    if (!response)
        return MHD_NO;

    MHD_add_response_header(response, "Location", location);
    MHD_add_response_header(response, "Content-Type", "text/html");
    ret = MHD_queue_response(connection, MHD_HTTP_FOUND, response);
    MHD_destroy_response(response);
    return ret;
}

// Handle login attempt
int login_handler(struct MHD_Connection *connection, const char *username, const char *password) {
    char *sql = "SELECT password FROM users WHERE username=?";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
        return send_redirect(connection, "/internal_server_error");

    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_TRANSIENT);

    int step = sqlite3_step(stmt);
    if (step == SQLITE_ROW) {
        const char *correct_password = (const char *)sqlite3_column_text(stmt, 0);
        sqlite3_finalize(stmt);
        if (strcmp(password, correct_password) == 0) {
            return send_redirect(connection, "/success");
        } else {
            return send_redirect(connection, "/cant_login");
        }
    } else if (step == SQLITE_DONE) {
        sqlite3_finalize(stmt);
        return send_redirect(connection, "/cant_login");
    } else {
        sqlite3_finalize(stmt);
        return send_redirect(connection, "/internal_server_error");
    }
}

// Handle incoming requests
int answer_to_connection(void *cls, struct MHD_Connection *connection,
                         const char *url, const char *method,
                         const char *version, const char *upload_data,
                         size_t *upload_data_size, void **con_cls) {
    if (strcmp(method, "GET") == 0 && strcmp(url, "/") == 0) {
        const char *login_form =
            "<html><body><form action='/login' method='post'>"
            "Username: <input type='text' name='username'><br>"
            "Password: <input type='password' name='password'><br>"
            "<input type='submit' value='Login'></form></body></html>";
        return send_redirect(connection, login_form);
    } else if (strcmp(method, "POST") == 0 && strcmp(url, "/login") == 0) {
        char *username = MHD_lookup_connection_value(connection, MHD_POSTDATA_KIND, "username");
        char *password = MHD_lookup_connection_value(connection, MHD_POSTDATA_KIND, "password");
        return login_handler(connection, username, password);
    }

    return send_redirect(connection, "/internal_server_error");
}

int main() {
    struct MHD_Daemon *daemon;

    if (sqlite3_open("login.db", &db) != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }

    // Ensure the 'users' table exists
    char *error;
    if (sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY, username TEXT UNIQUE, password TEXT);",
                     NULL, NULL, &error) != SQLITE_OK) {
        fprintf(stderr, "Failed to create table: %s\n", error);
        sqlite3_free(error);
        sqlite3_close(db);
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
