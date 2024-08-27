#include <kore/kore.h>
#include <kore/http.h>
#include <sqlite3.h>

// Prototype for handling the buy_order route
int buy_order(struct http_request *);

// Function to insert stock data into the database
static int insert_stock_data(const char *stock_name, int quantity) {
    sqlite3 *db;
    char *err_message = NULL;
    int result = 0;
    const char *db_path = "path_to_your_database.db";

    if (sqlite3_open(db_path, &db) != SQLITE_OK) {
        kore_log(LOG_ERR, "Can't open database: %s", sqlite3_errmsg(db));
        return 0;  // Return 0 if the database cannot be opened
    }

    char sql[256];
    snprintf(sql, sizeof(sql), "INSERT INTO stocks (name, quantity) VALUES (?, ?)");
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, stock_name, -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 2, quantity);
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

// HTTP request handler for "/buy_order"
int buy_order(struct http_request *req) {
    char stock_name[100];
    int quantity;
    
    http_populate_post(req);

    if (http_argument_get_string(req, "stock_name", stock_name, sizeof(stock_name)) &&
        http_argument_get_int(req, "quantity", &quantity)) {
        
        if (insert_stock_data(stock_name, quantity)) {
            // Redirect user to the stock_view page
            http_response_header(req, "location", "/stock_view");
            http_response(req, 302, NULL, 0);  // 302 Found - redirection
            return KORE_RESULT_OK;
        } else {
            http_response(req, 500, NULL, 0);  // 500 Internal Server Error
            return KORE_RESULT_ERROR;
        }
    } else {
        http_response(req, 400, NULL, 0);  // 400 Bad Request
        return KORE_RESULT_ERROR;
    }
}

// Main function with kore.io configuration setup
int main(void) {
    kore_log_register("stock_app", KORE_LOG_ERR, KORE_NO_LOGGING);
    kore_module_load(NULL, 0, NULL);
    kore_server_start();
    return 0;
}
