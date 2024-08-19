#include <microhttpd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PORT 8888

// Function to respond with a simple web page
int send_html_response(struct MHD_Connection *connection, const char *page) {
    struct MHD_Response *response = MHD_create_response_from_buffer(strlen(page), (void *)page, MHD_RESPMEM_PERSISTENT);
    int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    MHD_destroy_response(response);
    return ret;
}

// Handler for connection requests
int answer_to_connection(void *cls, struct MHD_Connection *connection,
                         const char *url, const char *method, const char *version,
                         const char *upload_data, size_t *upload_data_size, void **con_cls) {
    static int dummy;
    if (&dummy != *con_cls) {
        // Do not respond on the first call
        *con_cls = &dummy;
        return MHD_YES;
    }
    *con_cls = NULL;  // Reset when done

    if (strcmp(method, "POST") == 0 && strcmp(url, "/upload") == 0) {
        if (*upload_data_size != 0) {
            FILE *fp = fopen("uploads/uploaded_file.pdf", "wb");
            if (!fp) return MHD_NO;
            fwrite(upload_data, 1, *upload_data_size, fp);
            fclose(fp);
            *upload_data_size = 0;
            return MHD_YES;
        } else {
            const char *page = "<html><body>File has been uploaded successfully.</body></html>";
            return send_html_response(connection, page);
        }
    }

    return send_html_response(connection, "Invalid request.");
}

int main() {
    struct MHD_Daemon *daemon;

    daemon = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY, PORT, NULL, NULL,
                              &answer_to_connection, NULL, MHD_OPTION_END);
    if (NULL == daemon) {
        fprintf(stderr, "Failed to start the server.\n");
        return 1;
    }

    printf("Server running on port %d. Press enter to terminate.\n", PORT);
    getchar();

    MHD_stop_daemon(daemon);
    return 0;
}
