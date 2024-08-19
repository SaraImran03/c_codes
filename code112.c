#include <microhttpd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PORT 8888

// Handler for HTTP requests
static int answer_to_connection(void *cls, struct MHD_Connection *connection,
                                const char *url, const char *method,
                                const char *version, const char *upload_data,
                                size_t *upload_data_size, void **con_cls) {
    struct MHD_Response *response;
    int ret;
    FILE *fp;
    char command[256];
    char output[1024] = {0};
    char final_output[4096] = "Ping results:\n";

    if (strcmp(url, "/ping") == 0) {
        // Get the URL parameter from the connection
        const char *ping_url = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "url");
        if (ping_url == NULL) {
            response = MHD_create_response_from_buffer(strlen("URL parameter missing"),
                                                       (void *)"URL parameter missing", MHD_RESPMEM_PERSISTENT);
            ret = MHD_queue_response(connection, MHD_HTTP_BAD_REQUEST, response);
            MHD_destroy_response(response);
            return ret;
        }

        // Prepare and execute the ping command
        snprintf(command, sizeof(command), "ping -c 4 %s", ping_url);
        fp = popen(command, "r");
        if (fp == NULL) {
            response = MHD_create_response_from_buffer(strlen("Failed to execute command"),
                                                       (void *)"Failed to execute command", MHD_RESPMEM_PERSISTENT);
            ret = MHD_queue_response(connection, MHD_HTTP_INTERNAL_SERVER_ERROR, response);
            MHD_destroy_response(response);
            return ret;
        }

        // Read the output from the ping command
        while (fgets(output, sizeof(output), fp) != NULL) {
            strcat(final_output, output);
        }
        pclose(fp);

        // Send the ping command output as the response
        response = MHD_create_response_from_buffer(strlen(final_output), (void *)final_output, MHD_RESPMEM_MUST_COPY);
        ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
        MHD_destroy_response(response);
        return ret;
    }

    // Respond with 404 Not Found for other paths
    response = MHD_create_response_from_buffer(strlen("Not Found"), (void *)"Not Found", MHD_RESPMEM_PERSISTENT);
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
