#include <kore/kore.h>
#include <kore/http.h>
#include <sqlite3.h>

int unsubscribe_request(struct http_request *);

int unsubscribe_email(const char *email) {
    sqlite3 *db;
    char *err_message = NULL;
    int result = -1;
    const char *db_path = "path_to_your_database.db";

    if (sqlite3_open(db_path, &db) == SQLITE_OK) {
        char sql[256];
        
        // Check if the email exists in the database
        snprintf(sql, sizeof(sql), "SELECT COUNT(*) FROM subscribers WHERE email = '%s'", email);
        sqlite3_stmt *stmt;
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
            if (sqlite3_step(stmt) == SQLITE_ROW && sqlite3_column_int(stmt, 0) > 0) {
                // Email exists, proceed to delete
                snprintf(sql, sizeof(sql), "DELETE FROM subscribers WHERE email = '%s'", email);
                if (sqlite3_exec(db, sql, NULL, NULL, &err_message) != SQLITE_OK) {
                    fprintf(stderr, "SQL error: %s\n", err_message);
                    sqlite3_free(err_message);
                } else {
                    result = 1;  // Email removed
                }
            } else {
                result = 0;  // Email not found
            }
            sqlite3_finalize(stmt);
        } else {
            fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        }
        sqlite3_close(db);
    } else {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    }

    return result;
}

int unsubscribe_request(struct http_request *req) {
    char email[256];

    http_populate_post(req);

    if (http_argument_get_string(req, "email", email, sizeof(email))) {
        int status = unsubscribe_email(email);
        if (status == 1) {
            kore_response_printf(req, HTTP_STATUS_200, "Email %s has been unsubscribed successfully.", email);
        } else if (status == 0) {
            kore_response_printf(req, HTTP_STATUS_200, "Email %s was not found in our database.", email);
        } else {
            kore_response_printf(req, HTTP_STATUS_500, "An error occurred while processing your request.");
        }
    } else {
        kore_response_printf(req, HTTP_STATUS_400, "Invalid request, email not provided.");
    }

    return (KORE_RESULT_OK);
}

int main(void) {
    kore_log_register("unsubscribe_app", KORE_LOG_ERR, KORE_NO_LOGGING);
    kore_module_load(NULL, 0, NULL);
    kore_server_start();
    return 0;
}
