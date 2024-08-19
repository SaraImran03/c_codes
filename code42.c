#include <microhttpd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PORT 8888

// Simulated session data
typedef struct {
    char username[50];
    char email[100];
    char ssn[12];  // Format: XXX-XX-XXXX
} UserData;

// Example "session" data
UserData currentUser = {
    "user123",
    "user123@example.com",
    "123-45-6789"
};

int info(struct MHD_Connection *connection) {
    char response[300];
    snprintf(response, sizeof(response), "Username: %s, Email: %s, SSN: %s",
             currentUser.username, currentUser.email, currentUser.ssn);

    struct MHD_Response *mhd_response = MHD_create_response_from_buffer(strlen(response), (void*)response, MHD_RESPMEM_MUST_COPY);
    if (!mhd_response)
        return MHD_NO;

    MHD_add_response_header(mhd_response, "Content-Type", "text/plain");
    int ret = MHD_queue_response(connection, MHD_HTTP_OK, mhd_response);
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

    // Return 404 Not Found for other routes
    const char *notFoundMsg = "Not found";
    struct MHD_Response *response = MHD_create_response_from_buffer(strlen(notFoundMsg), (void *)notFoundMsg, MHD_RESPMEM_PERSISTENT);
    int ret = MHD_queue_response(connection, MHD_HTTP_NOT_FOUND, response);
    MHD_destroy_response(response);
    return ret;
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
