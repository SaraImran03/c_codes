#include <microhttpd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PORT 8888

// Structure to handle the POST data
struct connection_info_struct {
    FILE *fp;
    const char *filename;
    struct MHD_PostProcessor *postprocessor;
};

// Function to send a simple HTML response
int send_html_response(struct MHD_Connection *connection, const char *html, int status_code) {
    struct MHD_Response *response = MHD_create_response_from_buffer(strlen(html), (void *)html, MHD_RESPMEM_PERSISTENT);
    int ret = MHD_queue_response(connection, status_code, response);
    MHD_destroy_response(response);
    return ret;
}

// Callback to iterate over the POST data
int iterate_post(void *coninfo_cls, enum MHD_ValueKind kind, const char *key,
                 const char *filename, const char *content_type,
                 const char *transfer_encoding, const char *data, uint64_t off, size_t size) {
    struct connection_info_struct *con_info = coninfo_cls;

    if (0 == strcmp(key, "file")) {
        if (!con_info->fp) {
            // Open the file for writing in the uploads directory
            char filepath[1024];
            snprintf(filepath, sizeof(filepath), "uploads/%s", filename);
            con_info->fp = fopen(filepath, "ab");
            if (!con_info->fp) return MHD_NO;
        }
        if (size > 0) {
            if (!fwrite(data, size, 1, con_info->fp))
                return MHD_NO;
        }
        return MHD_YES;
    }
    return MHD_YES;
}

// Function to handle connection requests
int answer_to_connection(void *cls, struct MHD_Connection *connection,
                         const char *url, const char *method,
                         const char *version, const char *upload_data,
                         size_t *upload_data_size, void **con_cls) {
    if (NULL == *con_cls) {
        // Allocate our structure for the duration of the request
        struct connection_info_struct *con_info;
        con_info = malloc(sizeof(struct connection_info_struct));
        if (NULL == con_info) return MHD_NO;
        con_info->fp = NULL;

        if (0 == strcmp(method, "POST")) {
            con_info->postprocessor = MHD_create_post_processor(connection, 1024, iterate_post, (void *)con_info);

            if (NULL == con_info->postprocessor) {
                free(con_info);
                return MHD_NO;
            }

            *con_cls = (void *)con_info;

            return MHD_YES;
        }
    }

    if (0 == strcmp(method, "POST")) {
        struct connection_info_struct *con_info = *con_cls;

        if (*upload_data_size != 0) {
            MHD_post_process(con_info->postprocessor, upload_data, *upload_data_size);
            *upload_data_size = 0;
            return MHD_YES;
        } else if (NULL != con_info->fp) {
            fclose(con_info->fp);
            con_info->fp = NULL;
            // Respond that the file has been uploaded successfully
            return send_html_response(connection, "<html><body>File uploaded successfully.</body></html>", MHD_HTTP_OK);
        }
    }

    // Default response for GET and finished POST
    return send_html_response(connection, "<html><body>Upload PDF file <form action=\"/\" method=\"post\" enctype=\"multipart/form-data\"><input type=\"file\" name=\"file\"><input type=\"submit\"></form></body></html>", MHD_HTTP_OK);
}

int main() {
    struct MHD_Daemon *daemon;

    daemon = MHD_start_daemon(MHD_USE_INTERNAL_POLLING_THREAD, PORT, NULL, NULL,
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
