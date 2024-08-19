#include <microhttpd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PORT 8888

// Fake user data
typedef struct {
    char username[50];
    char email[100];
    char ssn[12];  // SSN is typically formatted as XXX-XX-XXXX
} UserInfo;

// Example user
UserInfo user = {
    "john_doe",
    "john.doe@example.com",
    "123-45-6789"
};

int serve_info_page(struct MHD_Connection *connection) {
    const char *page_format =
        "<html><body>"
        "<h1>User Information</h1>"
        "<p>Username: %s</p>"
        "<p>Email: %s</p>"
        "<p>Last 4 digits of SSN: %.4s</p>"
        "</body></html>";

    char *response;
    struct MHD_Response *mhd_response;
    int ret;

    // Create the response page with user information
    asprintf(&response, page_format, user.username, user.email, user.ssn + 7); // user.ssn + 7 to skip the first 7 characters

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
        return serve_info_page(connection);
    }

    return MHD_NO;  // Return 404 for other routes
}

int main() {
    struct MHD_Daemon *daemon;

    daemon = MHD_start_daemon(MHD_USE_INTERNAL_POLLING_THREAD, PORT, NULL, NULL,
                              &answer_to_connection, NULL, MHD_OPTION_END);
    if (NULL == daemon) return 1;

    printf("Server running on port %d. Press enter to terminate.\n", PORT);
    getchar(); // Wait for user input to terminate server

    MHD_stop_daemon(daemon);
    return 0;
}
