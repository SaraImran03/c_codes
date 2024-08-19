#include <microhttpd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

#define PORT 8888

sqlite3 *db;

int send_redirect_response(struct MHD_Connection *connection, const char *location) {
    struct MHD_Response *response;
    int ret;

    response = MHD_create_response_from_buffer(0, "", MHD_RESPMEM_PERSISTENT);
    MHD_add_response_header(response, "Location", location);
    ret = MHD_queue_response(connection, MHD_HTTP_FOUND, response);
    MHD_destroy_response(response);
    return ret;
}

int authenticate_user(struct MHD_Connection *connection, const char *username, const char *password) {
    const char *sql = "SELECT password FROM users WHERE username=?";
    sqlite3_stmt *stmt;
    int rc;

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return send_redirect_response(connection, "/login.html");
    }

    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const char *db_password = (const char *)sqlite3_column_text(stmt, 0);
        if (strcmp(password, db_password) == 0) {
            sqlite3_finalize(stmt);
            return send_redirect_response(connection, "/dashboard.html");
        }
    }

    sqlite3_finalize(stmt);
    return send_redirect_response(connection, "/login.html");
}

int handle_request(void *cls, struct MHD_Connection *connection,
                   const char *url, const char *method,
                   const char *version, const char *upload_data,
                   size_t *upload_data_size, void **con_cls) {
    if (strcmp(method, "POST") == 0 && strcmp(url, "/login") == 0) {
        char *username = MHD_lookup_connection_value(connection, MHD_POSTDATA_KIND, "username");
        char *password = MHD_lookup_connection_value(connection, MHD_POSTDATA_KIND, "password");
        return authenticate_user(connection, username, password);
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
    if (sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY, username TEXT, password TEXT);",
                     NULL, NULL, &error_msg) != SQLITE_OK) {
        fprintf(stderr, "Failed to create table: %s\n", error_msg);
        sqlite3_free(error_msg);
        sqlite3_close(db);
        return 1;
    }

    daemon = MHD_start_daemon(MHD_USE_INTERNAL_POLLING_THREAD, PORT, NULL, NULL,
                              &handle_request, NULL, MHD_OPTION_END);
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
