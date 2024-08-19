#include <microhttpd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <yaml.h>

#define PORT 5000

// Function to send a response
static int send_response(struct MHD_Connection *connection, const char *data, int status_code) {
    struct MHD_Response *response = MHD_create_response_from_buffer(strlen(data), (void *)data, MHD_RESPMEM_MUST_COPY);
    int ret = MHD_queue_response(connection, status_code, response);
    MHD_destroy_response(response);
    return ret;
}

// Handler for connection requests
static int answer_to_connection(void *cls, struct MHD_Connection *connection,
                                const char *url, const char *method,
                                const char *version, const char *upload_data,
                                size_t *upload_data_size, void **con_cls) {
    if (0 != strcmp(method, "GET")) {
        return send_response(connection, "Only GET method is accepted", MHD_HTTP_METHOD_NOT_ALLOWED);
    }

    const char *payload = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "payload");
    if (!payload) {
        return send_response(connection, "Parameter 'payload' is missing", MHD_HTTP_BAD_REQUEST);
    }

    // Parse the payload as YAML
    yaml_parser_t parser;
    yaml_document_t document;
    yaml_parser_initialize(&parser);
    yaml_parser_set_input_string(&parser, (const unsigned char *)payload, strlen(payload));

    if (!yaml_parser_load(&parser, &document)) {
        yaml_parser_delete(&parser);
        return send_response(connection, "Failed to parse YAML", MHD_HTTP_BAD_REQUEST);
    }

    // Check for 'type' in the YAML document
    yaml_node_t *root_node = yaml_document_get_root_node(&document);
    if (root_node->type == YAML_MAPPING_NODE) {
        for (yaml_node_pair_t *pair = root_node->data.mapping.pairs.start;
             pair < root_node->data.mapping.pairs.top; pair++) {
            yaml_node_t *key = yaml_document_get_node(&document, pair->key);
            yaml_node_t *value = yaml_document_get_node(&document, pair->value);

            if (strcmp((char *)key->data.scalar.value, "type") == 0) {
                if (strcmp((char *)value->data.scalar.value, "Create") == 0) {
                    yaml_parser_delete(&parser);
                    yaml_document_delete(&document);
                    return send_response(connection, "Operation not allowed for type 'Create'", MHD_HTTP_FORBIDDEN);
                } else if (strcmp((char *)value->data.scalar.value, "Update") == 0) {
                    for (yaml_node_pair_t *inner_pair = root_node->data.mapping.pairs.start;
                         inner_pair < root_node->data.mapping.pairs.top; inner_pair++) {
                        yaml_node_t *inner_key = yaml_document_get_node(&document, inner_pair->key);
                        yaml_node_t *inner_value = yaml_document_get_node(&document, inner_pair->value);

                        if (strcmp((char *)inner_key->data.scalar.value, "name") == 0 &&
                            strcmp((char *)inner_value->data.scalar.value, "UnsafeDeserialization") == 0) {
                            yaml_parser_delete(&parser);
                            yaml_document_delete(&document);
                            return send_response(connection, "Update operation detected for UnsafeDeserialization", MHD_HTTP_OK);
                        }
                    }
                }
            }
        }
    }

    yaml_parser_delete(&parser);
    yaml_document_delete(&document);
    return send_response(connection, "YAML parsed but no actionable content found", MHD_HTTP_OK);
}

int main() {
    struct MHD_Daemon *daemon = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY, PORT, NULL, NULL,
                                                 &answer_to_connection, NULL, MHD_OPTION_END);
    if (NULL == daemon) {
        fprintf(stderr, "Failed to start the server on port %d\n", PORT);
        return EXIT_FAILURE;
    }

    printf("Server running on port %d. Press enter to terminate.\n", PORT);
    getchar();

    MHD_stop_daemon(daemon);
    return EXIT_SUCCESS;
}
