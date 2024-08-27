#include <kore/kore.h>
#include <kore/http.h>
#include <sqlite3.h>

int page_root(struct http_request *);
int post(struct http_request *);

// Function to insert a message into the database
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

// Handler for the main page and displaying the form
int page_root(struct http_request *req) {
    // HTML form for submitting a message
    char *response = 
        "<html>"
        "<head><title>Message Board</title></head>"
        "<body>"
        "<h1>Submit your message</h1>"
        "<form action='/post' method='post'>"
        "Username: <input type='text' name='username'><br>"
        "Message: <textarea name='message'></textarea><br>"
        "<input type='submit' value='Submit'>"
        "</form>"
        "</body>"
        "</html>";

    http_response(req, 200, response, strlen(response));
    return KORE_RESULT_OK;
}

// Handler for posting a message
int post(struct http_request *req) {
    char username[100];
    char message[1024];

    http_populate_post(req);

    if (http_argument_get_string(req, "username", username, sizeof(username)) &&
        http_argument_get_string(req, "message", message, sizeof(message))) {
        
        if (insert_user_message_in_db(username, message)) {
            http_response_header(req, "location", "/");
            http_response(req, 302, NULL, 0); // Redirect back to the main page
        } else {
            http_response(req, 500, "Failed to insert message", strlen("Failed to insert message"));
        }
    } else {
        http_response(req, 400, "Missing fields", strlen("Missing fields"));
    }

    return KORE_RESULT_OK;
}
