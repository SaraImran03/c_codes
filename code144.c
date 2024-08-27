#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>

// Function to check if the email exists and unsubscribe
int check_and_unsubscribe(const char *email) {
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int result = 0; // Default return 0 if email does not exist
    const char *db_path = "path_to_your_database.db";
    char *err_message = NULL;
    char command[256];

    // Open the database
    if (sqlite3_open(db_path, &db) != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 0; // If can't open database, assume email doesn't exist
    }

    // Prepare the SQL query to check if the email exists
    const char *sql = "SELECT EXISTS(SELECT 1 FROM subscribers WHERE email = ?)";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 0; // If can't prepare statement, assume email doesn't exist
    }

    // Bind the email to the SQL query
    sqlite3_bind_text(stmt, 1, email, -1, SQLITE_STATIC);

    // Execute the query
    if (sqlite3_step(stmt) == SQLITE_ROW && sqlite3_column_int(stmt, 0) == 1) {
        // Email exists, call the PHP script
        snprintf(command, sizeof(command), "php unsubscribe_script.php \"%s\"", email);
        result = system(command);
        if (result == 0) {
            // PHP script succeeded
            result = 1;
        } else {
            // PHP script failed
            result = 0;
        }
    }

    // Clean up
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return result;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <email>\n", argv[0]);
        return 1;
    }

    int status = check_and_unsubscribe(argv[1]);
    printf("Result: %d\n", status);
    return status;
}
