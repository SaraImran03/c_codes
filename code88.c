#include <microhttpd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <yaml.h>

#define PORT 8888

// Predefined product prices
const double prices[] = {10.0, 15.5, 20.0, 25.75, 30.0};

// Function to parse the YAML and calculate the total price
double calculate_price(const char *yaml_text) {
    yaml_parser_t parser;
    yaml_document_t document;
    yaml_node_t *root, *key, *value;
    int product_code = 0;
    int quantity = 0;

    yaml_parser_initialize(&parser);
    yaml_parser_set_input_string(&parser, (const unsigned char *)yaml_text, strlen(yaml_text));

    if (!yaml_parser_load(&parser, &document)) {
        fprintf(stderr, "Failed to parse YAML\n");
        return -1;
    }

    root = yaml_document_get_root_node(&document);
    if (!root || root->type != YAML_MAPPING_NODE) {
        yaml_document_delete(&document);
        yaml_parser_delete(&parser);
        return -1;
    }

    for (yaml_node_pair_t *pair = root->data.mapping.pairs.start; pair < root->data.mapping.pairs.top; pair++) {
        key = yaml_document_get_node(&document, pair->key);
        value = yaml_document_get_node(&document, pair->value);

        if (strcmp((char *)key->data.scalar.value, "product_code") == 0) {
            product_code = atoi((char *)value->data.scalar.value);
        } else if (strcmp((char *)key->data.scalar.value, "quantity") == 0) {
            quantity = atoi((char *)value->data.scalar.value);
        }
    }

    yaml_document_delete(&document);
    yaml_parser_delete(&parser);

    if (product_code > 0 && product_code <= sizeof(prices)/sizeof(prices[0])) {
        return prices[product_code - 1] * quantity;
    }

    return -1;
}

// Function to handle incoming HTTP requests
static int answer_to_connection(void *cls, struct MHD_Connection *connection,
                                const char *url, const char *method,
                                const char *version, const char *upload_data,
                                size_t *upload_data_size, void **con_cls) {
    if (strcmp(url, "/api/calculate_price") != 0 || strcmp(method, "GET") != 0) {
        return MHD_NO;
    }

    const char *yaml_payload = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "payload");
    if (!yaml_payload) {
        const char *msg = "No payload provided";
        struct MHD_Response *response = MHD_create_response_from_buffer(strlen(msg), (void *)msg, MHD_RESPMEM_PERSISTENT);
        return MHD_queue_response(connection, MHD_HTTP_BAD_REQUEST, response);
    }

    double price = calculate_price(yaml_payload);
    if (price < 0) {
        const char *msg = "Error calculating price";
        struct MHD_Response *response = MHD_create_response_from_buffer(strlen(msg), (void *)msg, MHD_RESPMEM_PERSISTENT);
        return MHD_queue_response(connection, MHD_HTTP_BAD_REQUEST, response);
    }

    char result[256];
    snprintf(result, sizeof(result), "Total price: %.2f", price);
    struct MHD_Response *response = MHD_create_response_from_buffer(strlen(result), (void *)result, MHD_RESPMEM_PERSISTENT);
    return MHD_queue_response(connection, MHD_HTTP_OK, response);
}

int main() {
    struct MHD_Daemon *daemon = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY, PORT, NULL, NULL,
                                                 &answer_to_connection, NULL, MHD_OPTION_END);
    if (daemon == NULL) {
        fprintf(stderr, "Failed to start the server.\n");
        return EXIT_FAILURE;
    }

    printf("Server running on port %d. Press enter to terminate.\n", PORT);
    getchar();

    MHD_stop_daemon(daemon);
    return EXIT_SUCCESS;
}
