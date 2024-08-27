#include <mysql/mysql.h>
#include <stdio.h>
#include <stdlib.h>

int is_moderator(const char *username) {
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;

    const char *server = getenv("DB_HOST");
    const char *user = getenv("DB_USER");
    const char *password = getenv("DB_PASS");
    const char *database = getenv("DB_NAME");

    conn = mysql_init(NULL);

    // Connect to the database
    if (!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0)) {
        fprintf(stderr, "%s\n", mysql_error(conn));
        return -1; // Return -1 to indicate connection failure
    }

    // Prepare the SQL query to check if the user is a moderator
    char query[256];
    snprintf(query, sizeof(query), "SELECT is_moderator FROM users WHERE username = '%s'", username);

    // Execute the query
    if (mysql_query(conn, query)) {
        fprintf(stderr, "%s\n", mysql_error(conn));
        mysql_close(conn);
        return -1; // Return -1 to indicate query execution failure
    }

    res = mysql_use_result(conn);

    // Check the result: assuming is_moderator is an INT field in the database
    if ((row = mysql_fetch_row(res)) != NULL) {
        int is_mod = atoi(row[0]);
        mysql_free_result(res);
        mysql_close(conn);
        return is_mod; // Return 1 if moderator, 0 otherwise
    }

    // Cleanup
    mysql_free_result(res);
    mysql_close(conn);
    return 0; // Return 0 if no such user exists
}

int main() {
    const char *username = "exampleUser"; // This should be dynamically provided based on your use case
    int result = is_moderator(username);
    if (result == -1) {
        printf("Error occurred.\n");
    } else {
        printf("User %s is %s a moderator.\n", username, result ? "": "not");
    }
    return 0;
}
