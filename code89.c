#include <microhttpd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PORT 5000

// Assuming some predefined prices for simplicity (e.g., product codes 1, 2, 3)
double prices[] = {10.99, 20.99, 15.49};

// Function to get price based on product code
double get_price(int product_code, int quantity) {
    if (product_code > 0 && product_code <= sizeof(prices) / sizeof(prices[0])) {
        return prices[product_code - 1] * quantity;
    }
    return -1.0; // Indicate invalid product code or quantity
}

// Handler for web requests
static int answer_to_connection(void *cls, struct MHD_Connection *connection,
                                const char *url, const char *method, 
                                const char *version, const char *upload_data,
                                size_t *upload_data_size, void **con_cls) {
    if (strcmp(url, "/calculate_price") == 0 && strcmp(method, "GET") == 0) {
        const char *product_code_str = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "product_code");
        const char *quantity_str = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "quantity");

        if (product_code_str == NULL || quantity_str == NULL) {
            const char *error = "Missing product_code or quantity";
            struct MHD_Response *response = MHD_create_response_from_buffer(strlen(error), (void *)error, MHD_RESPMEM_PERSISTENT);
            return MHD_queue_response(connection, MHD_HTTP_BAD_REQUEST, response);
        }

        int product_code = atoi(product_code_str);
        int quantity = atoi(quantity_str);
        double price = get_price(product_code, quantity);

        if (price < 0) {
            const char *error = "Invalid product code or quantity";
            struct MHD_Response *response = MHD_create_response_from_buffer(strlen(error), (void *)error, MHD_RESPMEM_PERSISTENT);
            return MHD_queue_response(connection, MHD_HTTP_BAD_REQUEST, response);
        }

        char result[64];
        snprintf(result, sizeof(result), "Total price: %.2f", price);
        struct MHD_Response *response = MHD_create_response_from_buffer(strlen(result), (void *)result, MHD_RESPMEM_PERSISTENT);
        return MHD_queue_response(connection, MHD_HTTP_OK, response);
    }

    return MHD_NO;  // For other routes or methods
}

int main() {
    struct MHD_Daemon *daemon = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY, PORT, NULL, NULL,
                                                 &answer_to_connection, NULL, MHD_OPTION_END);
    if (daemon == NULL) {
        fprintf(stderr, "Failed to start the server.\n");
        return EXIT_FAILURE;
    }

    printf("Server running on port %d. Press enter to terminate.\n", PORT);
    getchar();  // Wait for user input to terminate server

    MHD_stop_daemon(daemon);
    return EXIT_SUCCESS;
}
