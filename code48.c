#include <microhttpd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PORT 8888

// Helper function to send the contents of the file as a response
int send_file_response(struct MHD_Connection *connection, const char *file_path) {
    FILE *file = fopen(file_path, "rb");
    if (!file) {
        perror("Error opening file");
        const char *error_msg = "File not found";
        struct MHD_Response *response = MHD_create_response_from_buffer(strlen(error_msg), (void *)error_msg, MHD_RESPMEM_PERSISTENT);
        return MHD_queue_response(connection, MHD_HTTP_NOT_FOUND, response);
    }

    // Determine the file size
    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    rewind(file);

    // Read the file into memory
    char *data = malloc(file_size);
    if (fread(data, 1, file_size, file) != file_size) {
        fclose(file);
        free(data);
        return MHD_NO; // Failed to read the file
    }

    // Create a response with the file data
    struct MHD_Response *response = MHD_create_response_from_buffer(file_size, data, MHD_RESPMEM_MUST_FREE);
    int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    MHD_destroy_response(response);

    fclose(file);
    return ret;
}

// Handler function to process incoming connections
int answer_to_connection(void *cls, struct MHD_Connection *connection,
                         const char *url, const char *method,
                         const char *version, const char *upload_data,
                         size_t *upload_data_size, void **con_cls) {
    if (strcmp(method, "GET") == 0 && strcmp(url, "/img") == 0) {
        const char *filename = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "file");
        if (!filename) {
            const char *error_msg = "Missing file parameter";
            struct MHD_Response *response = MHD_create_response_from_buffer(strlen(error_msg), (void *)error_msg, MHD_RESPMEM_PERSISTENT);
            return MHD_queue_response(connection, MHD_HTTP_BAD_REQUEST, response);
        }

        char file_path[1024];
        snprintf(file_path, sizeof(file_path), "images/%s", filename);
        return send_file_response(connection, file_path);
    }

    return MHD_NO; // 404 Not Found for any other endpoint or method
}

int main() {
    struct MHD_Daemon *daemon;

    daemon = MHD_start_daemon(MHD_USE_INTERNAL_POLLING_THREAD, PORT, NULL, NULL,
                              &answer_to_connection, NULL, MHD_OPTION_END);
    if (NULL == daemon) {
        fprintf(stderr, "Failed to start the server.\n");
        return EXIT_FAILURE;
    }

    printf("Server running on port %d. Press enter to terminate.\n", PORT);
    getchar(); // Wait for user input to terminate server

    MHD_stop_daemon(daemon);
    return EXIT_SUCCESS;
}
