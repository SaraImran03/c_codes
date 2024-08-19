#include <microhttpd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PORT 8888
#define UPLOAD_DIR "uploads/"

// Function to respond with a simple HTML form
int send_html_form(struct MHD_Connection *connection) {
    const char *page = "<html><head><title>Upload File</title></head><body>"
                       "<h1>Upload File</h1>"
                       "<form action=\"/upload\" method=\"post\" enctype=\"multipart/form-data\">"
                       "<input type=\"file\" name=\"file\" required>"
                       "<input type=\"submit\" value=\"Upload\">"
                       "</form>"
                       "</body></html>";

    struct MHD_Response *response = MHD_create_response_from_buffer(strlen(page), (void *)page, MHD_RESPMEM_PERSISTENT);
    int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    MHD_destroy_response(response);
    return ret;
}

// Callback function to handle the POST data
int iterate_post(void *coninfo_cls, enum MHD_ValueKind kind, const char *key,
                 const char *filename, const char *content_type,
                 const char *transfer_encoding, const char *data, uint64_t off, size_t size) {
    if (0 == strcmp(key, "file")) {
        FILE *fp;
        char filepath[1024];

        snprintf(filepath, sizeof(filepath), "%s%s", UPLOAD_DIR, filename);
        if (off == 0) { // Start of the new file
            fp = fopen(filepath, "wb");
            if (!fp) {
                perror("Cannot open file");
                return MHD_NO;
            }
        } else { // Continuing to write to the file
            fp = fopen(filepath, "ab");
            if (!fp) {
                perror("Cannot open file");
                return MHD_NO;
            }
        }

        if (size > 0) {
            if (fwrite(data, 1, size, fp) != size) {
                fclose(fp);
                return MHD_NO; // Stop the connection
            }
        }

        fclose(fp);
    }
    return MHD_YES;
}

// Function to handle connections and dispatch handlers
int answer_to_connection(void *cls, struct MHD_Connection *connection,
                         const char *url, const char *method,
                         const char *version, const char *upload_data,
                         size_t *upload_data_size, void **con_cls) {
    if (0 == strcmp(method, "GET")) {
        return send_html_form(connection);
    } else if (0 == strcmp(method, "POST")) {
        struct MHD_PostProcessor *processor = MHD_create_post_processor(connection, 8192, iterate_post, NULL);
        int ret = MHD_post_process(processor, upload_data, *upload_data_size);
        if (*upload_data_size == 0) {
            MHD_destroy_post_processor(processor);
            const char *page = "<html><body>Upload successful.</body></html>";
            struct MHD_Response *response = MHD_create_response_from_buffer(strlen(page), (void *)page, MHD_RESPMEM_PERSISTENT);
            ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
            MHD_destroy_response(response);
        }
        *upload_data_size = 0;
        return ret;
    }
    return MHD_NO;  // Method not handled
}

int main() {
    struct MHD_Daemon *daemon;

    daemon = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY, PORT, NULL, NULL, &answer_to_connection, NULL, MHD_OPTION_END);
    if (NULL == daemon) {
        fprintf(stderr, "Failed to start the server.\n");
        return EXIT_FAILURE;
    }

    printf("Server running on port %d. Press enter to terminate.\n", PORT);
    getchar();

    MHD_stop_daemon(daemon);
    return EXIT_SUCCESS;
}
