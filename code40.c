#include <microhttpd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PORT 8888

// Fake session storage (simulating session data)
typedef struct {
    char username[256];
    char email[256];
    char ssn[10];  // Storing full SSN for this example (not recommended in production)
} SessionData;

// For simplicity, we're using a single session data instance
SessionData session = {
    .username = "user123",
    .email = "user123@example.com",
    .ssn = "123-45-6789"
};

int info(struct MHD_Connection *connection) {
    const char *page_format =
        "<html><body>"
        "Username: %s<br>"
        "Email: %s<br>"
        "Last 4 digits of SSN: %s"
        "</body></html>";

    char *response;
    int ret;
    struct MHD_Response *mhd_response;

    // Format the page with session data
    asprintf(&response, page_format, session.username, session.email, session.ssn + 6); // ssn+6 skips the first 6 characters

    mhd_response = MHD_create_response_from_buffer(strlen(response), (void *)response, MHD_RESPMEM_MUST_FREE);
    if (!mhd_response) {
        free(response);
        return MHD_NO;
    }

    MHD_add_response_header(mhd_response, "Content-Type", "text/html");
    ret = MHD_queue_response(connection, MHD_HTTP_OK, mhd_response);
    MHD_destroy_response(mhd_response);
    return ret;
}

int answer_to_connection(void *cls, struct MHD_Connection *connection,
                         const char *url, const char *method,
                         const char *version, const char *upload_data,
                         size_t *upload_data_size, void **con_cls) {
    if (strcmp(method, "GET") == 0 && strcmp(url, "/info") == 0) {
        return info(connection);
    }

    return MHD_NO;
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
