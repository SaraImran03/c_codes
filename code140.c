#include <stdio.h>
#include <sqlite3.h>

int username_exists(const char *username, sqlite3 *db) {
    sqlite3_stmt *stmt;
    int result = 0;
    const char *sql = "SELECT COUNT(*) FROM users WHERE username = ?";

    // Prepare the SQL statement
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        // Bind the username to the query
        sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);

        // Execute the query
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            result = sqlite3_column_int(stmt, 0); // Get the count
        }
        sqlite3_finalize(stmt); // Finalize the statement to prevent memory leaks
    } else {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
    }

    return result > 0; // Return true if count is more than 0
}

int main() {
    sqlite3 *db;
    char *err_message = NULL;
    const char *database_path = "path_to_your_database.db";
    const char *username = "exampleUser"; // This should be dynamically provided based on your use case

    // Open the database
    if (sqlite3_open(database_path, &db) != SQLITE_OK) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }

    // Check if the username exists
    int exists = username_exists(username, db);
    printf("Username %s does %sexist in the database.\n", username, exists ? "" : "not ");

    // Close the database connection
    sqlite3_close(db);
    return 0;
}
