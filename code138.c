#include <kore/kore.h>
#include <kore/http.h>
#include <sqlite3.h>

int check_moderator(struct http_request *);

// Function to execute SQL query and check moderator status
int is_moderator(const char *username, sqlite3 *db) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT is_moderator FROM users WHERE username = ?";
    int result = 0, is_mod = 0;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            is_mod = sqlite3_column_int(stmt, 0);
        }
        sqlite3_finalize(stmt);
    }
    return is_mod;
}

// Handler for the route "/check"
int check_moderator(struct http_request *req) {
    char username[100];
    http_populate_get(req);
    if (http_argument_get_string(req, "username", &username, sizeof(username))) {
        sqlite3 *db;
        if (sqlite3_open("path_to_your_database.db", &db) == SQLITE_OK) {
            int mod_status = is_moderator(username, db);
            sqlite3_close(db);

            if (mod_status) {
                kore_response_printf(req, HTTP_STATUS_200, "User %s is a moderator.", username);
            } else {
                kore_response_printf(req, HTTP_STATUS_200, "User %s is not a moderator.", username);
            }
        } else {
            kore_response_printf(req, HTTP_STATUS_500, "Failed to connect to database.");
        }
    } else {
        kore_response_printf(req, HTTP_STATUS_400, "Missing username parameter.");
    }
    return (KORE_RESULT_OK);
}

int main(int argc, char *argv[]) {
    kore_log_register("moderator_check", KORE_LOG_ERR, KORE_NO_LOGGING);
    kore_module_load(NULL, argc, argv);
    kore_server_start();
    return (0);
}
