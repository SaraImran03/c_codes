#include <stdio.h>
#include <stdlib.h>
#include <mysql/mysql.h>

// Database connection parameters
#define DB_HOST "localhost"
#define DB_USER "your_username"
#define DB_PASS "your_password"
#define DB_NAME "your_database"

// Function to initialize a connection to the database
MYSQL* init_db() {
    MYSQL *conn = mysql_init(NULL);
    if (conn == NULL) {
        fprintf(stderr, "MySQL initialization failed: %s\n", mysql_error(conn));
        return NULL;
    }

    if (mysql_real_connect(conn, DB_HOST, DB_USER, DB_PASS, DB_NAME, 0, NULL, 0) == NULL) {
        fprintf(stderr, "MySQL connection failed: %s\n", mysql_error(conn));
        mysql_close(conn);
        return NULL;
    }

    return conn;
}

// Function to create a table if it does not exist
int create_table(MYSQL *conn) {
    char *sql = "CREATE TABLE IF NOT EXISTS images ("
                "id INT AUTO_INCREMENT PRIMARY KEY, "
                "data LONGBLOB NOT NULL)";
    if (mysql_query(conn, sql)) {
        fprintf(stderr, "Failed to create table: %s\n", mysql_error(conn));
        return 0;
    }
    return 1;
}

// Function to store an image file into the database
int store_image(MYSQL *conn, const char *file_path) {
    FILE *file = fopen(file_path, "rb");
    if (!file) {
        perror("Failed to open file");
        return 0;
    }

    // Get the file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    // Read the file into a buffer
    unsigned char *buffer = (unsigned char *)malloc(file_size);
    if (buffer == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        fclose(file);
        return 0;
    }

    fread(buffer, 1, file_size, file);
    fclose(file);

    // Prepare the insert query
    char *sql = "INSERT INTO images (data) VALUES(?)";
    MYSQL_STMT *stmt = mysql_stmt_init(conn);
    if (!stmt) {
        fprintf(stderr, "mysql_stmt_init() failed\n");
        free(buffer);
        return 0;
    }

    if (mysql_stmt_prepare(stmt, sql, strlen(sql))) {
        fprintf(stderr, "mysql_stmt_prepare() failed\n");
        mysql_stmt_close(stmt);
        free(buffer);
        return 0;
    }

    MYSQL_BIND bind[1];
    memset(bind, 0, sizeof(bind));

    // Bind the buffer
    bind[0].buffer_type = MYSQL_TYPE_BLOB;
    bind[0].buffer = buffer;
    bind[0].buffer_length = file_size;

    if (mysql_stmt_bind_param(stmt, bind)) {
        fprintf(stderr, "mysql_stmt_bind_param() failed\n");
        mysql_stmt_close(stmt);
        free(buffer);
        return 0;
    }

    // Execute the statement
    if (mysql_stmt_execute(stmt)) {
        fprintf(stderr, "mysql_stmt_execute() failed: %s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        free(buffer);
        return 0;
    }

    mysql_stmt_close(stmt);
    free(buffer);
    return 1;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <image_file_path>\n", argv[0]);
        return EXIT_FAILURE;
    }

    MYSQL *conn = init_db();
    if (!conn) return EXIT_FAILURE;

    if (!create_table(conn)) {
        mysql_close(conn);
        return EXIT_FAILURE;
    }

    if (!store_image(conn, argv[1])) {
        mysql_close(conn);
        return EXIT_FAILURE;
    }

    printf("Image stored successfully.\n");

    mysql_close(conn);
    return EXIT_SUCCESS;
}
