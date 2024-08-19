#include <microhttpd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <json-c/json.h>

#define PORT 8888

// Function to send HTTP responses
int send_response(struct MHD_Connection *connection, const char *data, unsigned int status_code) {
    struct MHD_Response *response = MHD_create_response_from_buffer(strlen(data), (void *)data, MHD_RESPMEM_MUST_COPY);
    int ret = MHD_queue_response(connection, status_code, response);
    MHD_destroy_response(response);
    return ret;
}

// Handler for the POST data
int post_iterator(void *cls, enum MHD_ValueKind kind, const char *key,
                  const char *filename, const char *content_type,
                  const char *transfer_encoding, const char *data, uint64_t off, size_t size) {
    if (size > 0) {
        struct json_object *parsed_json;
        struct json_object *name;

        parsed_json = json_tokener_parse(data);
        if (json_object_object_get_ex(parsed_json, "name", &name)) {
            const char *name_str = json_object_get_string(name);
            send_response(cls, name_str, MHD_HTTP_OK);
            json_object_put(parsed_json); // Free JSON object
            return MHD_YES;
        } else {
            json_object_put(parsed_json); // Free JSON object
            send_response(cls, "Name field not found", MHD_HTTP_BAD_REQUEST);
            return MHD_NO;
        }
    }
    return MHD_YES;
}

// Function to handle connections
int answer_to_connection(void *cls, struct MHD_Connection *connection,
                         const char *url, const char *method,
                         const char *version, const char *upload_data,
                         size_t *upload_data_size, void **con_cls) {
    static int dummy;

    if (0 != strcmp(method, "POST")) {
        return MHD_NO; // Reject non-POST methods
    }

    if (&dummy != *con_cls) {
        // Do not respond on the first call
        *con_cls = &dummy;
        return MHD_YES;
    }

    if (*upload_data_size != 0) {
        post_iterator(connection, MHD_POSTDATA_KIND, NULL, NULL, NULL, NULL, upload_data, 0, *upload_data_size);
        *upload_data_size = 0;
        return MHD_YES;
    }

    *con_cls = NULL;
    return MHD_NO;
}

int main() {
    struct MHD_Daemon *daemon;

    daemon = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY, PORT, NULL, NULL,
                              &answer_to_connection, NULL, MHD_OPTION_END);
    if (NULL == daemon) {
        fprintf(stderr, "Failed to start the server.\n");
        return EXIT_FAILURE;
    }

    printf("Server running on port %d. Press enter to terminate.\n", PORT);
    getchar();

    MHD_stop_daemon(daemon);
    return EXIT_SUCCESS;
}
