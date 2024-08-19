#include <microhttpd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define PORT 8888

int redirect_invalid_url(void *cls, struct MHD_Connection *connection,
                         const char *url, const char *method,
                         const char *version, const char *upload_data,
                         size_t *upload_data_size, void **con_cls) {
    const char *responseText;
    int ret;
    struct MHD_Response *response;

    if (strstr(url, "example.com") != NULL) {
        // Redirect to the URL
        const char *redirect_url = url; // assuming URL is full path including domain
        response = MHD_create_response_from_buffer(strlen(redirect_url),
                                                   (void*)redirect_url, MHD_RESPMEM_PERSISTENT);
        MHD_add_response_header(response, "Location", redirect_url);
        ret = MHD_queue_response(connection, MHD_HTTP_MOVED_PERMANENTLY, response);
    } else {
        // Return an error message
        responseText = "Invalid URL. Only URLs containing 'example.com' are allowed.";
        response = MHD_create_response_from_buffer(strlen(responseText),
                                                   (void*)responseText, MHD_RESPMEM_PERSISTENT);
        ret = MHD_queue_response(connection, MHD_HTTP_BAD_REQUEST, response);
    }

    MHD_destroy_response(response);
    return ret;
}

int main() {
    struct MHD_Daemon *daemon;

    daemon = MHD_start_daemon(MHD_USE_INTERNAL_POLLING_THREAD, PORT, NULL, NULL,
                              &redirect_invalid_url, NULL, MHD_OPTION_END);
    if (NULL == daemon) return 1;

    printf("Server running on port %d\n", PORT);
    getchar();  // Press Enter to stop the server

    MHD_stop_daemon(daemon);
    return 0;
}
