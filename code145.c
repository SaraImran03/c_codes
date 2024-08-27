#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>

// Function prototypes
int buy_stock(const char *stock_name, int quantity);
void buy_function(const char *stock_name);

// Function to insert stock data into the database and call buy_function
int buy_stock(const char *stock_name, int quantity) {
    sqlite3 *db;
    char *err_message = NULL;
    int result = 0;
    const char *db_path = "your_database.db";

    // Open the database
    if (sqlite3_open(db_path, &db) != SQLITE_OK) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return 0;  // Return 0 if database cannot be opened
    }

    // Prepare the SQL insert statement
    char sql[256];
    snprintf(sql, sizeof(sql), "INSERT INTO stocks (stock_name, quantity) VALUES ('%s', %d)", stock_name, quantity);

    // Execute the SQL statement
    if (sqlite3_exec(db, sql, 0, 0, &err_message) != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_message);
        sqlite3_free(err_message);
        result = 0;  // Return 0 if insert fails
    } else {
        result = 1;  // Return 1 if insert succeeds
        buy_function(stock_name);  // Call buy_function to process the stock purchase
    }

    // Close the database connection
    sqlite3_close(db);

    return result;
}

// Function that processes further actions with the stock
void buy_function(const char *stock_name) {
    printf("Processing the purchase of the stock: %s\n", stock_name);
    // Implement additional functionality as required
}

int main() {
    // Example usage
    const char *stock_name = "AAPL";
    int quantity = 15;

    if (buy_stock(stock_name, quantity)) {
        printf("Stock purchase transaction completed successfully.\n");
    } else {
        printf("Failed to complete stock purchase transaction.\n");
    }

    return 0;
}
