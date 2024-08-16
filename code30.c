#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <microhttpd.h>
#include <pcre.h>

#define PORT 8888

// Compile the regular expression and check if it matches the target
int validate_target(const char *target) {
    const char *pattern = "^[A-Za-z0-9-]+\\.";
    const char *error;
    int erroffset;
    int rc;
    int ovector[30];
    
    pcre *re = pcre_compile(pattern, 0, &error, &erroffset, NULL);
    if (re == NULL) {
        fprintf(stderr, "PCRE compilation failed at offset %d: %s\n", erroffset, error);
        return 0; // Indicates error in regex compilation
    }

    rc = pcre_exec(re, NULL, target, strlen(target), 0, 0, ovector, 30);
    pcre_free(re); // Free the compiled pattern

    if (rc < 0)
        return 0; // No match
    else
        return 1; // Match found
}

// Handler for incoming HTTP requests
int answer_to_connection(void *cls, struct MHD_Connection *connection,
                         const char *url, const char *method,
                         const char *version, const char *upload_data,
                         size_t *upload_data_size, void **con_cls) {
    struct MHD_Response *response;
    int ret;
    const char *target = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "target");
    
    if (target == NULL || !validate_target(target)) {
        // If target is not valid or missing, redirect to root
        response = MHD_create_response_from_buffer(strlen("/"),
                                                   (void *) "/", MHD_RESPMEM_PERSISTENT);
        ret = MHD_queue_response(connection, MHD_HTTP_FOUND, response);
    } else {
        // Valid target, redirect to it
        response = MHD_create_response_from_buffer(strlen(target),
                                                   (void *)target, MHD_RESPMEM_PERSISTENT);
        ret = MHD_queue_response(connection, MHD_HTTP_FOUND, response);
    }

    MHD_destroy_response(response);
    return ret;
}

int main() {
    struct MHD_Daemon *daemon;

    daemon = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY, PORT, NULL, NULL,
                              &answer_to_connection, NULL, MHD_OPTION_END);
    if (daemon == NULL)
        return 1;

    printf("Server running on port %d\n", PORT);
    getchar(); // Wait for an enter key press to terminate

    MHD_stop_daemon(daemon);
    return 0;
}
