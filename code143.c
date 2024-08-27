#include <kore/kore.h>
#include <kore/http.h>
#include <sqlite3.h>

int unsubscribe_endpoint(struct http_request *);

// Function to check the email in the database and unsubscribe if present
int unsubscribe_email(const char *email) {
    sqlite3 *db;
    char *err_message = NULL;
    int result = -1; // Default to an error state
    const char *db_path = "path_to_your_database.db";

    if (sqlite3_open(db_path, &db) == SQLITE_OK) {
        char sql[256];
        snprintf(sql, sizeof(sql), "SELECT COUNT(*) FROM subscribers WHERE email = '%s'", email);
        sqlite3_stmt *stmt;

        if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
            if (sqlite3_step(stmt) == SQLITE_ROW && sqlite3_column_int(stmt, 0) > 0) {
                // Email exists, proceed to delete
                sqlite3_finalize(stmt); // finalize the previous statement before a new exec
                snprintf(sql, sizeof(sql), "DELETE FROM subscribers WHERE email = '%s'", email);
                if (sqlite3_exec(db, sql, NULL, NULL, &err_message) != SQLITE_OK) {
                    fprintf(stderr, "SQL error: %s\n", err_message);
                    sqlite3_free(err_message);
                } else {
                    result = 1; // Email removed
                }
            } else {
                result = 0; // Email not found
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

int unsubscribe_endpoint(struct http_request *req) {
    char *email;
    
    // Getting the 'email' parameter from the URL query
    if (http_request_get_param(req, "email", &email) == KORE_RESULT_OK) {
        int status = unsubscribe_email(email);
        if (status == 1) {
            kore_response_printf(req, HTTP_STATUS_200, "Email %s has been successfully unsubscribed.", email);
        } else if (status == 0) {
            kore_response_printf(req, HTTP_STATUS_200, "Email %s is not subscribed.", email);
        } else {
            kore_response_printf(req, HTTP_STATUS_500, "An error occurred while processing your request.");
        }
    } else {
        kore_response_printf(req, HTTP_STATUS_400, "Bad request, email parameter is missing.");
    }

    return (KORE_RESULT_OK);
}

int main(void) {
    // Kore configuration and server start
    kore_log_register("unsubscribe_app", KORE_LOG_ERR, KORE_NO_LOGGING);
    kore_module_load(NULL, 0, NULL);
    kore_server_start();
    return 0;
}
