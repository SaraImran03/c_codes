#include <kore/kore.h>
#include <kore/http.h>
#include <sqlite3.h>

int page_post(struct http_request *);
int page_root(struct http_request *);

static int insert_user_message_in_db(const char *username, const char *message) {
    sqlite3 *db;
    char *err_message = NULL;
    const char *db_path = "your_database.db";
    int result = 0;

    if (sqlite3_open(db_path, &db) != SQLITE_OK) {
        kore_log(LOG_ERR, "Can't open database: %s", sqlite3_errmsg(db));
        return 0;
    }

    char sql[256];
    snprintf(sql, sizeof(sql), "INSERT INTO messages (username, message) VALUES (?, ?)");
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, message, -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_DONE) {
            result = 1;
        }
        sqlite3_finalize(stmt);
    } else {
        kore_log(LOG_ERR, "SQL error: %s", sqlite3_errmsg(db));
    }

    sqlite3_close(db);
    return result;
}

int page_post(struct http_request *req) {
    char username[100];
    char message[256];

    http_populate_post(req);

    if (http_argument_get_string(req, "username", username, sizeof(username)) &&
        http_argument_get_string(req, "message", message, sizeof(message))) {
        
        if (insert_user_message_in_db(username, message)) {
            http_response_header(req, "location", "/");
            http_response(req, 302, NULL, 0);
        } else {
            http_response(req, 500, NULL, 0);
        }
    } else {
        http_response(req, 400, NULL, 0);
    }

    return KORE_RESULT_OK;
}

int page_root(struct http_request *req) {
    sqlite3 *db;
    sqlite3_stmt *stmt;
    const char *db_path = "your_database.db";
    char *html_response = malloc(1024 * sizeof(char));
    strcpy(html_response, "<html><body><h1>Messages</h1>");

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
