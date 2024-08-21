#include <microhttpd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define PORT 8888

// Function to check the credentials
int check_credentials(const char *username, const char *password) {
    const char *validUsername = "admin";
    const char *validPassword = "pass123";

    return (strcmp(username, validUsername) == 0 && strcmp(password, validPassword) == 0);
}

// Handler for incoming GET requests
int answer_to_connection(void *cls, struct MHD_Connection *connection,
                         const char *url, const char *method,
                         const char *version, const char *upload_data,
                         size_t *upload_data_size, void **con_cls) {
    const char *page = "<html><body>False</body></html>";
    struct MHD_Response *response;
    int ret;

    if (strcmp(url, "/check_mod") == 0 && strcmp(method, "GET") == 0) {
        const char *username = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "username");
        const char *password = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "password");

        if (username && password && check_credentials(username, password)) {
            page = "<html><body>True</body></html>";
        }

        response = MHD_create_response_from_buffer(strlen(page), (void *)page, MHD_RESPMEM_PERSISTENT);
        ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
        MHD_destroy_response(response);
        return ret;
    }

    // Respond with 404 if the URL is not /check_mod
    page = "<html><body>404 Not Found</body></html>";
    response = MHD_create_response_from_buffer(strlen(page), (void *)page, MHD_RESPMEM_PERSISTENT);
    ret = MHD_queue_response(connection, MHD_HTTP_NOT_FOUND, response);
    MHD_destroy_response(response);
    return ret;
}

int main() {
    struct MHD_Daemon *daemon;

    daemon = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY, PORT, NULL, NULL,
                              &answer_to_connection, NULL, MHD_OPTION_END);
    if (NULL == daemon)
        return 1;

    printf("Server running on port %d\n", PORT);
    getchar();  // Press enter to stop the server

    MHD_stop_daemon(daemon);
    return 0;
}
