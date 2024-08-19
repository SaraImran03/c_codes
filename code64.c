#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <microhttpd.h>
#include <mysql/mysql.h>

#define PORT 8888
#define UPLOAD_FOLDER "uploads/"

MYSQL *conn;

// Initialize MySQL connection
int init_db() {
    conn = mysql_init(NULL);
    if (!conn) {
        fprintf(stderr, "MySQL initialization failed: %s\n", mysql_error(conn));
        return 0;
    }

    if (!mysql_real_connect(conn, "localhost", "username", "password", "database", 0, NULL, 0)) {
        fprintf(stderr, "MySQL connection failed: %s\n", mysql_error(conn));
        return 0;
    }

    return 1;
}

// Function to save file path in the database
int save_filepath_in_db(const char *filepath) {
    char query[1024];
    snprintf(query, sizeof(query), "INSERT INTO pdf_files (filepath) VALUES ('%s')", filepath);
    if (mysql_query(conn, query)) {
        fprintf(stderr, "Failed to insert data: %s\n", mysql_error(conn));
        return 0;
    }
    return 1;
}

// Function to handle the POST data and save the uploaded PDF file
int upload_pdf(struct MHD_Connection *connection, const char *upload_data, size_t *upload_data_size, char *filename) {
    static FILE *fp;
    static int file_length = 0;

    if (!*upload_data_size) {
        // Finish the upload process
        fclose(fp);
        printf("Received file: %s (%d bytes)\n", filename, file_length);

        // Save the file path to the database
        char filepath[1024];
        snprintf(filepath, sizeof(filepath), "%s%s", UPLOAD_FOLDER, filename);
        if (!save_filepath_in_db(filepath)) {
            return MHD_NO;
        }

        return MHD_YES;
    }

    if (!fp) {
        // Open file for writing
        char full_path[1024];
        snprintf(full_path, sizeof(full_path), "%s%s", UPLOAD_FOLDER, filename);
        fp = fopen(full_path, "wb");
        if (!fp) {
            perror("Error opening file");
            return MHD_NO;
        }
    }

    // Write data to the file
    fwrite(upload_data, 1, *upload_data_size, fp);
    file_length += *upload_data_size;
    *upload_data_size = 0;
    return MHD_YES;
}

// Main function to handle incoming requests
int answer_to_connection(void *cls, struct MHD_Connection *connection,
                         const char *url, const char *method,
                         const char *version, const char *upload_data,
                         size_t *upload_data_size, void **con_cls) {
    if (strcmp(method, "POST") == 0 && strcmp(url, "/upload_pdf") == 0) {
        // Extract filename from the connection (assumed to be passed as a POST parameter for simplicity)
        const char *filename = MHD_lookup_connection_value(connection, MHD_POSTDATA_KIND, "filename");
        return upload_pdf(connection, upload_data, upload_data_size, (char *)filename);
    }
    return MHD_NO;
}

int main() {
    struct MHD_Daemon *daemon;

    if (!init_db()) {
        fprintf(stderr, "Database initialization failed.\n");
        return EXIT_FAILURE;
    }

    daemon = MHD_start_daemon(MHD_USE_INTERNAL_POLLING_THREAD, PORT, NULL, NULL,
                              &answer_to_connection, NULL, MHD_OPTION_END);
    if (NULL == daemon) {
        fprintf(stderr, "Failed to start the server.\n");
        return EXIT_FAILURE;
    }

    printf("Server running on port %d. Press enter to terminate.\n", PORT);
    getchar();

    MHD_stop_daemon(daemon);
    mysql_close(conn);
    return EXIT_SUCCESS;
}
