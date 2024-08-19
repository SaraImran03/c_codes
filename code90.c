#include <microhttpd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <yaml.h>

#define PORT 5000

// Predefined prices for simplicity
double prices[] = {10.99, 20.99, 30.99};  // Prices for product codes 1, 2, 3, respectively

// Function to get the price for a given product code
double get_price_for_product_code(int product_code) {
    if (product_code > 0 && product_code <= sizeof(prices) / sizeof(prices[0])) {
        return prices[product_code - 1];
    }
    return -1.0;  // Invalid product code
}

// Parse YAML from a string and calculate the total price
double calculate_price_from_yaml(const char* yaml_text) {
    yaml_parser_t parser;
    yaml_document_t document;
    int product_code = 0;
    int quantity = 0;
    double total_price = -1.0;

    yaml_parser_initialize(&parser);
    yaml_parser_set_input_string(&parser, (const unsigned char*)yaml_text, strlen(yaml_text));

    if (!yaml_parser_load(&parser, &document)) {
        fprintf(stderr, "Failed to parse YAML\n");
        yaml_parser_delete(&parser);
        return -1.0;
    }

    // Assume root is a mapping node
    yaml_node_t *root_node = yaml_document_get_root_node(&document);
    if (root_node && root_node->type == YAML_MAPPING_NODE) {
        for (yaml_node_pair_t *pair = root_node->data.mapping.pairs.start;
             pair < root_node->data.mapping.pairs.top; pair++) {
            yaml_node_t *key = yaml_document_get_node(&document, pair->key);
            yaml_node_t *value = yaml_document_get_node(&document, pair->value);
            
            if (strcmp((char *)key->data.scalar.value, "product_code") == 0) {
                product_code = atoi((char *)value->data.scalar.value);
            } else if (strcmp((char *)key->data.scalar.value, "quantity") == 0) {
                quantity = atoi((char *)value->data.scalar.value);
            }
        }

        double price = get_price_for_product_code(product_code);
        if (price >= 0) {
            total_price = price * quantity;
        }
    }

    yaml_document_delete(&document);
    yaml_parser_delete(&parser);

    return total_price;
}

// Web request handler
static int answer_to_connection(void *cls, struct MHD_Connection *connection,
                                const char *url, const char *method,
                                const char *version, const char *upload_data,
                                size_t *upload_data_size, void **con_cls) {
    if (strcmp(url, "/calculate_price") != 0 || strcmp(method, "GET") != 0) {
        return MHD_NO; // Reject non-GET or different endpoint
    }

    const char *yaml_payload = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "payload");
    if (!yaml_payload) {
        const char *error_msg = "No YAML payload provided";
        struct MHD_Response *response = MHD_create_response_from_buffer(strlen(error_msg),
                                                                        (void *)error_msg, MHD_RESPMEM_PERSISTENT);
        return MHD_queue_response(connection, MHD_HTTP_BAD_REQUEST, response);
    }

    double price = calculate_price_from_yaml(yaml_payload);
    if (price < 0) {
        const char *error_msg = "Error calculating price";
        struct MHD_Response *response = MHD_create_response_from_buffer(strlen(error_msg),
                                                                        (void *)error_msg, MHD_RESPMEM_PERSISTENT);
        return MHD_queue_response(connection, MHD_HTTP_BAD_REQUEST, response);
    }

    char result[128];
    snprintf(result, sizeof(result), "total_price: %.2f\n", price);
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
    getchar(); // Wait for user input to terminate server

    MHD_stop_daemon(daemon);
    return EXIT_SUCCESS;
}
