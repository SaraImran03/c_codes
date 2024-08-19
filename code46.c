#include <microhttpd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PORT 8888

// Function to serve an image file
int img(struct MHD_Connection *connection, const char *filename) {
    char filepath[1024];
    snprintf(filepath, sizeof(filepath), "images/%s", filename);
    FILE *file = fopen(filepath, "rb");
    if (!file) {
        perror("Error opening file");
        // Return 404 Not Found if the file does not exist
        const char *error_msg = "File not found";
        struct MHD_Response *response = MHD_create_response_from_buffer(strlen(error_msg),
                                                                        (void *)error_msg, MHD_RESPMEM_PERSISTENT);
        return MHD_queue_response(connection, MHD_HTTP_NOT_FOUND, response);
    }

    // Get the size of the file
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    rewind(file);

    // Read the file into a buffer
    char *data = malloc(size);
    if (size != fread(data, 1, size, file)) {
        fclose(file);
        free(data);
        return MHD_NO; // Return an error if the file cannot be read
    }

    // Create a response from the data
    struct MHD_Response *response = MHD_create_response_from_buffer(size, data, MHD_RESPMEM_MUST_FREE);
    MHD_add_response_header(response, "Content-Type", "image/jpeg"); // assuming JPEG for simplicity
    int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    MHD_destroy_response(response);
    fclose(file);

    return ret;
}

// Callback function to handle connections
int answer_to_connection(void *cls, struct MHD_Connection *connection,
                         const char *url, const char *method,
                         const char *version, const char *upload_data,
                         size_t *upload_data_size, void **con_cls) {
    if (strcmp(method, "GET") == 0 && strcmp(url, "/img") == 0) {
        const char *name = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "name");
        if (name)
            return img(connection, name);
        else {
            const char *error_msg = "Name parameter is missing";
            struct MHD_Response *response = MHD_create_response_from_buffer(strlen(error_msg),
                                                                            (void *)error_msg, MHD_RESPMEM_PERSISTENT);
            return MHD_queue_response(connection, MHD_HTTP_BAD_REQUEST, response);
        }
    }

    return MHD_NO; // Return 404 Not Found for any other requests
}

int main() {
    struct MHD_Daemon *daemon;

    daemon = MHD_start_daemon(MHD_USE_INTERNAL_POLLING_THREAD, PORT, NULL, NULL,
                              &answer_to_connection, NULL, MHD_OPTION_END);
    if (NULL == daemon) return 1;

    printf("Server running on port %d. Press enter to terminate.\n", PORT);
    getchar();

    MHD_stop_daemon(daemon);
    return 0;
}
