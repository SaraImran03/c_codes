#include <kore/kore.h>
#include <kore/http.h>
#include <sqlite3.h>

int post(struct http_request *);
int display_messages(struct http_request *);

// Function to insert a message into the database
static int insert_user_message_in_db(const char *username, const char *message) {
    sqlite3 *db;
    char *err_message = NULL;
    int result = 0;
    const char *db_path = "your_database.db";

    if (sqlite3_open(db_path, &db) != SQLITE_OK) {
        kore_log(LOG_ERR, "Can't open database: %s", sqlite3_errmsg(db));
        return 0;  // Return 0 if the database cannot be opened
    }

    char sql[256];
    snprintf(sql, sizeof(sql), "INSERT INTO messages (username, message) VALUES (?, ?)");
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, message, -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_DONE) {
            result = 1;  // Success
        }
        sqlite3_finalize(stmt);
    } else {
        kore_log(LOG_ERR, "SQL error: %s", sqlite3_errmsg(db));
    }

    sqlite3_close(db);
    return result;
}

// HTTP request handler for "/post"
int post(struct http_request *req) {
    char username[100];
    char message[256];

    http_populate_post(req);

    if (http_argument_get_string(req, "username", username, sizeof(username)) &&
        http_argument_get_string(req, "message", message, sizeof(message))) {
        
        if (insert_user_message_in_db(username, message)) {
            // Redirect user to the main page
            http_response_header(req, "location", "/main");
            http_response(req, 302, NULL, 0);  // 302 Found - redirection
        } else {
            http_response(req, 500, NULL, 0);  // 500 Internal Server Error
        }
    } else {
        http_response(req, 400, NULL, 0);  // 400 Bad Request
    }

    return KORE_RESULT_OK;
}

// Function to display messages on the main page
int display_messages(struct http_request *req) {
    sqlite3 *db;
    sqlite3_stmt *stmt;
    const char *db_path = "your_database.db";
    char *html_response = malloc(1024 * sizeof(char));
    strcpy(html_response, "<html><body>");

    if (sqlite3_open(db_path, &db) == SQLITE_OK) {
        char sql[] = "SELECT username, message FROM messages ORDER BY rowid DESC";

        if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                strcat(html_response, "<p><strong>");
                strcat(html_response, (const char *)sqlite3_column_text(stmt, 0));
                strcat(html_response, ":</strong> ");
                strcat(html_response, (const char *)sqlite3_column_text(stmt, 1));
                strcat(html_response, "</p>");
            }
            sqlite3_finalize(stmt);
        }
        sqlite3_close(db);
    }

    strcat(html_response, "</body></html>");
    http_response(req, 200, html_response, strlen(html_response));
    free(html_response);
    return KORE_RESULT_OK;
}
