#include <microhttpd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PORT 8888

// Function to check if the uploaded file is of a safe type
int file_is_safe_type(const char *filename) {
    // Stub function that always returns true
    return 1;
}

// Function to process the POST data and save the uploaded file
int upload_file(const char *filename, const char *data, size_t data_size) {
    if (!file_is_safe_type(filename)) {
        printf("Unsafe file type attempted to upload.\n");
        return 0;
    }

    // Ensure the uploads directory exists
    system("mkdir -p uploads");

    char filepath[1024];
    snprintf(filepath, sizeof(filepath), "uploads/%s", filename);

    FILE *file = fopen(filepath, "wb");
    if (!file) {
        perror("Failed to open file for writing");
        return 0;
    }

    fwrite(data, 1, data_size, file);
    fclose(file);

    return 1;
}

// Callback for handling POST data
int post_iterator(void *cls, enum MHD_ValueKind kind, const char *key,
                  const char *filename, const char *content_type,
                  const char *transfer_encoding, const char *data, uint64_t off, size_t size) {
    if (0 == strcmp(key, "file") && size > 0) {
        upload_file(filename, data, size);
    }
    return MHD_YES;
}

// Handler function to process requests
int answer_to_connection(void *cls, struct MHD_Connection *connection,
                         const char *url, const char *method,
                         const char *version, const char *upload_data,
                         size_t *upload_data_size, void **con_cls) {
    if (0 == strcmp(method, "POST")) {
        struct MHD_PostProcessor *post_processor = MHD_create_post_processor(connection, 1024, post_iterator, NULL);
        if (!post_processor) {
            fprintf(stderr, "Cannot allocate post processor!\n");
            return MHD_NO;
        }

        int ret = MHD_post_process(post_processor, upload_data, *upload_data_size);
        MHD_destroy_post_processor(post_processor);

        if (ret != MHD_YES) {
            fprintf(stderr, "Error handling POST data\n");
            return MHD_NO;
        }

        const char *page = "<html><body>File uploaded successfully.</body></html>";
        return send_html_response(connection, page, MHD_HTTP_OK);
    }

    // Default response for GET or other methods
    const char *page = "<html><body>"
                       "<form action=\"/\" method=\"post\" enctype=\"multipart/form-data\">"
                       "Select file: <input type=\"file\" name=\"file\" />"
                       "<input type=\"submit\" value=\"Upload\" />"
                       "</form>"
                       "</body></html>";
    return send_html_response(connection, page, MHD_HTTP_OK);
}

int main() {
    struct MHD_Daemon *daemon = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY, PORT, NULL, NULL,
                                                 &answer_to_connection, NULL, MHD_OPTION_END);
    if (NULL == daemon) {
        fprintf(stderr, "Failed to start the server.\n");
        return EXIT_FAILURE;
    }

    printf("Server running on port %d. Press enter to terminate.\n", PORT);
    getchar();

    MHD_stop_daemon(daemon);
    return EXIT_SUCCESS;
}

// Helper function to send HTML responses
int send_html_response(struct MHD_Connection *connection, const char *html, int status_code) {
    struct MHD_Response *response = MHD_create_response_from_buffer(strlen(html), (void *)html, MHD_RESPMEM_PERSISTENT);
    int ret = MHD_queue_response(connection, status_code, response);
    MHD_destroy_response(response);
    return ret;
}
