#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>

// Function to insert buy order into the database
int insert_buy_order(const char *username, const char *stock_name, int stock_quantity) {
    sqlite3 *db;
    char *err_message = NULL;
    int result = 0;
    const char *db_path = "your_database.db";

    // Open the database
    if (sqlite3_open(db_path, &db) != SQLITE_OK) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return 0;  // Return 0 if the database cannot be opened
    }

    // Prepare the SQL insert statement
    char sql[256];
    snprintf(sql, sizeof(sql), "INSERT INTO buy_orders (username, stock_name, quantity) VALUES ('%s', '%s', %d)",
             username, stock_name, stock_quantity);

    // Execute the SQL statement
    if (sqlite3_exec(db, sql, 0, 0, &err_message) != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_message);
        sqlite3_free(err_message);
        result = 0;  // Return 0 if insert fails
    } else {
        result = 1;  // Return 1 if insert succeeds
    }

    // Close the database connection
    sqlite3_close(db);

    return result;
}

int main() {
    char stock_name[100];
    int stock_quantity;
    char username[100];

    // Simulate form input in a command-line environment
    printf("Enter your username: ");
    scanf("%99s", username);
    printf("Enter stock name: ");
    scanf("%99s", stock_name);
    printf("Enter stock quantity: ");
    scanf("%d", &stock_quantity);

    // Insert buy order into the database
    if (insert_buy_order(username, stock_name, stock_quantity)) {
        printf("Buy order successfully inserted.\n");
        // Simulate redirection to profile page
        printf("Redirecting to profile page...\n");
    } else {
        printf("Failed to insert buy order.\n");
    }

    return 0;
}
