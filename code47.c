#include <microhttpd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h> // For checking file existence and attributes

#define PORT 8888

// Check if the file exists and is not a directory
int file_exists(const char *filename) {
    struct stat buffer;
    return (stat(filename, &buffer) == 0 && !S_ISDIR(buffer.st_mode));
}

// Function to serve the image file if it exists
int serve_image(struct MHD_Connection *connection, const char *filename) {
    char filepath[1024];
    snprintf(filepath, sizeof(filepath), "images/%s", filename);

    if (!file_exists(filepath)) {
        const char *msg = "File not found";
        struct MHD_Response *response = MHD_create_response_from_buffer(strlen(msg), (void*)msg, MHD_RESPMEM_PERSISTENT);
        return MHD_queue_response(connection, MHD_HTTP_NOT_FOUND, response);
    }

    FILE *file = fopen(filepath, "rb");
    if (file == NULL) {
        perror("Error opening file");
        return MHD_NO;
    }

    // Determine the size of the file
    fseek(file, 0, SEEK_END);
    size_t filesize = ftell(file);
    rewind(file);

    // Allocate memory and read the file into buffer
    char *filedata = malloc(filesize);
    if (filesize != fread(filedata, 1, filesize, file)) {
        fclose(file);
        free(filedata);
        return MHD_NO; // File reading error
    }

    // Create a response with the file data
    struct MHD_Response *response = MHD_create_response_from_buffer(filesize, filedata, MHD_RESPMEM_MUST_FREE);
    MHD_add_response_header(response, "Content-Type", "image/jpeg"); // You might want to dynamically determine the MIME type
    int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    MHD_destroy_response(response);
    fclose(file);

    return ret;
}

// Main connection handler
int answer_to_connection(void *cls, struct MHD_Connection *connection,
                         const char *url, const char *method,
                         const char *version, const char *upload_data,
                         size_t *upload_data_size, void **con_cls) {
    if (strcmp(method, "GET") == 0 && strncmp(url, "/image/", 7) == 0) {
        const char *filename = url + 7; // Skip the '/image/' part to get the filename
        return serve_image(connection, filename);
    }

    const char *error_msg = "Invalid request";
    struct MHD_Response *response = MHD_create_response_from_buffer(strlen(error_msg),
                                                                    (void *)error_msg, MHD_RESPMEM_PERSISTENT);
    return MHD_queue_response(connection, MHD_HTTP_BAD_REQUEST, response);
}

int main() {
    struct MHD_Daemon *daemon;
    daemon = MHD_start_daemon(MHD_USE_INTERNAL_POLLING_THREAD, PORT, NULL, NULL,
                              &answer_to_connection, NULL, MHD_OPTION_END);
    if (NULL == daemon) {
        return 1;
    }

    printf("Server running on port %d. Press enter to terminate.\n", PORT);
    getchar(); // Wait for user input to terminate server

    MHD_stop_daemon(daemon);
    return 0;
}
