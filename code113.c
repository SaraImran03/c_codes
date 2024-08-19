#include <microhttpd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PORT 8888

// Function that handles the network response
static int answer_to_connection(void *cls, struct MHD_Connection *connection,
                                const char *url, const char *method,
                                const char *version, const char *upload_data,
                                size_t *upload_data_size, void **con_cls) {
    struct MHD_Response *response;
    int ret;

    if (strcmp(url, "/ping") == 0) {
        const char *ping_url = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "url");

        if (ping_url == NULL) {
            response = MHD_create_response_from_buffer(strlen("URL parameter missing"),
                                                       "URL parameter missing", MHD_RESPMEM_PERSISTENT);
            ret = MHD_queue_response(connection, MHD_HTTP_BAD_REQUEST, response);
            MHD_destroy_response(response);
            return ret;
        }

        char command[256];
        snprintf(command, sizeof(command), "ping -c 1 %s", ping_url);
        int system_status = system(command);

        char *msg;
        if (WIFEXITED(system_status) && WEXITSTATUS(system_status) == 0) {
            msg = "URL is alive";
        } else {
            msg = "URL is not alive";
        }

        response = MHD_create_response_from_buffer(strlen(msg), msg, MHD_RESPMEM_PERSISTENT);
        ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
        MHD_destroy_response(response);
        return ret;
    }

    response = MHD_create_response_from_buffer(strlen("Not Found"),
                                               "Not Found", MHD_RESPMEM_PERSISTENT);
    ret = MHD_queue_response(connection, MHD_HTTP_NOT_FOUND, response);
    MHD_destroy_response(response);
    return ret;
}

int main() {
    struct MHD_Daemon *daemon;

    daemon = MHD_start_daemon(MHD_USE_INTERNAL_POLLING_THREAD, PORT, NULL, NULL,
                              &answer_to_connection, NULL, MHD_OPTION_END);
    if (NULL == daemon)
        return 1;

    printf("Server running on port %d. Press enter to terminate.\n", PORT);
    getchar();

    MHD_stop_daemon(daemon);
    return 0;
}
