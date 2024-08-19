#include <microhttpd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sqlite3.h>

#define PORT 8888

sqlite3 *db;

// Function to send HTML responses
int send_html_response(struct MHD_Connection *connection, const char *html_content, int status_code) {
    struct MHD_Response *response;
    int ret;

    response = MHD_create_response_from_buffer(strlen(html_content), (void *)html_content, MHD_RESPMEM_PERSISTENT);
    if (!response) return MHD_NO;

    MHD_add_response_header(response, "Content-Type", "text/html");
    ret = MHD_queue_response(connection, status_code, response);
    MHD_destroy_response(response);
    return ret;
}

// Login function that checks username and password against the database
int login_function(struct MHD_Connection *connection, const char *username, const char *password) {
    char *sql = "SELECT * FROM users WHERE username=? AND password=?";
    sqlite3_stmt *stmt;

    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        sqlite3_finalize(stmt);
        return send_html_response(connection, "<html><body>Login successful! Redirecting to home page...</body></html>", MHD_HTTP_OK);
    } else {
        sqlite3_finalize(stmt);
        return send_html_response(connection, "<html><body>Login failed! Redirecting to login page...</body></html>", MHD_HTTP_FOUND);
    }
}

// Handler function for incoming requests
int answer_to_connection(void *cls, struct MHD_Connection *connection,
                         const char *url, const char *method,
                         const char *version, const char *upload_data,
                         size_t *upload_data_size, void **con_cls) {
    if (strcmp(method, "GET") == 0 && strcmp(url, "/login") == 0) {
        // Serve the HTML form
        FILE *file = fopen("login.html", "r");
        fseek(file, 0, SEEK_END);
        long fsize = ftell(file);
        rewind(file);

        char *html = malloc(fsize + 1);
        fread(html, 1, fsize, file);
        html[fsize] = 0;
        fclose(file);

        return send_html_response(connection, html, MHD_HTTP_OK);
    } else if (strcmp(method, "POST") == 0 && strcmp(url, "/login") == 0) {
        // Handle form data submitted
        char *username = MHD_lookup_connection_value(connection, MHD_POSTDATA_KIND, "username");
        char *password = MHD_lookup_connection_value(connection, MHD_POSTDATA_KIND, "password");

        return login_function(connection, username, password);
    }

    return MHD_NO;
}

int main() {
    struct MHD_Daemon *daemon;
    sqlite3_open("login.db", &db);

    // Ensure the 'users' table exists
    char *error;
    if (sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY, username TEXT, password TEXT);",
                     NULL, NULL, &error) != SQLITE_OK) {
        fprintf(stderr, "Failed to create table: %s\n", error);
        sqlite3_free(error);
    }

    daemon = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY, PORT, NULL, NULL,
                              &answer_to_connection, NULL, MHD_OPTION_END);
    if (NULL == daemon) return 1;

    printf("Server running on port %d\n", PORT);
    getchar();  // Press Enter to stop the server

    MHD_stop_daemon(daemon);
    sqlite3_close(db);
    return 0;
}
