#include <microhttpd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <json-c/json.h>

#define PORT 8888

// Function to send HTTP response
static int send_response(struct MHD_Connection *connection, const char *data) {
    struct MHD_Response *response = MHD_create_response_from_buffer(strlen(data), (void *)data, MHD_RESPMEM_MUST_COPY);
    int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    MHD_destroy_response(response);
    return ret;
}

// Handler for /api route
static int answer_to_connection(void *cls, struct MHD_Connection *connection, 
                                const char *url, const char *method, 
                                const char *version, const char *upload_data, 
                                size_t *upload_data_size, void **con_cls) {
    if (strcmp(url, "/api") != 0) return MHD_NO; // handle only /api route

    if (strcmp(method, "GET") == 0) {
        const char *payload = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "payload");
        if (payload == NULL) {
            return send_response(connection, "Payload not found.");
        }

        // Parse JSON from the payload
        struct json_object *parsed_json;
        struct json_object *name;
        
        parsed_json = json_tokener_parse(payload);
        if (json_object_object_get_ex(parsed_json, "name", &name)) {
            const char *name_str = json_object_get_string(name);
            char response[1024];
            snprintf(response, sizeof(response), "Name: %s", name_str);
            json_object_put(parsed_json); // Free JSON object
            return send_response(connection, response);
        } else {
            json_object_put(parsed_json); // Free JSON object
            return send_response(connection, "Name key not found in JSON.");
        }
    }

    return MHD_NO; // Unsupported method
}

int main() {
    struct MHD_Daemon *daemon;

    daemon = MHD_start_daemon(MHD_USE_INTERNAL_POLLING_THREAD, PORT, NULL, NULL, 
                              &answer_to_connection, NULL, MHD_OPTION_END);
    if (NULL == daemon) {
        fprintf(stderr, "Failed to start the server.\n");
        return EXIT_FAILURE;
    }

    printf("Server running on port %d\n", PORT);
    getchar(); // Press Enter to stop the server

    MHD_stop_daemon(daemon);
    return EXIT_SUCCESS;
}
