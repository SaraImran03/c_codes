#include <microhttpd.h>
#include <sqlite3.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define PORT 8888

sqlite3 *db;

int send_page(struct MHD_Connection *connection, const char *page, int status_code) {
    int ret;
    struct MHD_Response *response;

    response = MHD_create_response_from_buffer(strlen(page), (void *)page, MHD_RESPMEM_MUST_COPY);
    if (!response) return MHD_NO;

    MHD_add_response_header(response, "Content-Type", "text/html");
    ret = MHD_queue_response(connection, status_code, response);
    MHD_destroy_response(response);
    return ret;
}

int login_handler(void *cls, struct MHD_Connection *connection, const char *url,
                  const char *method, const char *version, const char *upload_data,
                  size_t *upload_data_size, void **con_cls) {
    if (strcmp(method, "GET") == 0) {
        return send_page(connection, "<html><body><form action='/login' method='post'>"
                                     "Username: <input type='text' name='username'><br>"
                                     "Password: <input type='password' name='password'><br>"
                                     "<input type='submit' value='Login'></form></body></html>", MHD_HTTP_OK);
    } else if (strcmp(method, "POST") == 0) {
        // Handling POST data here
        char *username = MHD_lookup_connection_value(connection, MHD_POSTDATA_KIND, "username");
        char *password = MHD_lookup_connection_value(connection, MHD_POSTDATA_KIND, "password");

        char *query = "SELECT * FROM users WHERE username=? AND password=?";
        sqlite3_stmt *stmt;
        sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
        sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, password, -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_ROW) {
            sqlite3_finalize(stmt);
            return send_page(connection, "<html><body>Logged in! Redirecting to dashboard...</body></html>", MHD_HTTP_OK);
        } else {
            sqlite3_finalize(stmt);
            return send_page(connection, "<html><body>Login failed! Redirecting to login page...</body></html>", MHD_HTTP_BAD_REQUEST);
        }
    }

    return MHD_NO;
}

int main() {
    struct MHD_Daemon *daemon;
    sqlite3_open("login.db", &db);

    // Database setup
    char *err_msg = 0;
    int rc = sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY, username TEXT, password TEXT);", 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
    }

    daemon = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY, PORT, NULL, NULL,
                              &login_handler, NULL, MHD_OPTION_END);
    if (NULL == daemon) return 1;

    printf("Server running on port %d\n", PORT);
    getchar();  // Press Enter to stop the server

    MHD_stop_daemon(daemon);
    sqlite3_close(db);
    return 0;
}
