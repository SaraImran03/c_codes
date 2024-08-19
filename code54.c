#include <microhttpd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PORT 8888

const char *login_page =
    "<html><body>"
    "<form action='/login' method='post'>"
    "Username: <input type='text' name='username'><br>"
    "Password: <input type='password' name='password'><br>"
    "<input type='submit' value='Login'>"
    "</form>"
    "</body></html>";

const char *user_page =
    "<html><body>"
    "<h1>Welcome User!</h1>"
    "</body></html>";

// Function to send HTML responses
int send_html_response(struct MHD_Connection *connection, const char *html, int status_code) {
    struct MHD_Response *response = MHD_create_response_from_buffer(strlen(html), (void *)html, MHD_RESPMEM_MUST_COPY);
    int ret = MHD_queue_response(connection, status_code, response);
    MHD_destroy_response(response);
    return ret;
}

// Handler for the login page (GET request)
int login_function(struct MHD_Connection *connection) {
    return send_html_response(connection, login_page, MHD_HTTP_OK);
}

// Handler for processing login credentials (POST request)
int do_login_function(struct MHD_Connection *connection, const char *username, const char *password) {
    // Check credentials (hardcoded for demonstration)
    if (strcmp(username, "admin") == 0 && strcmp(password, "admin123") == 0) {
        // Redirect to user page if credentials are correct
        return send_html_response(connection, user_page, MHD_HTTP_OK);
    } else {
        // Invalid credentials, reload login page
        return send_html_response(connection, login_page, MHD_HTTP_UNAUTHORIZED);
    }
}

// Main connection handler
int answer_to_connection(void *cls, struct MHD_Connection *connection,
                         const char *url, const char *method,
                         const char *version, const char *upload_data,
                         size_t *upload_data_size, void **con_cls) {
    if (strcmp(method, "GET") == 0 && strcmp(url, "/login") == 0) {
        return login_function(connection);
    } else if (strcmp(method, "POST") == 0 && strcmp(url, "/login") == 0) {
        const char *username = MHD_lookup_connection_value(connection, MHD_POSTDATA_KIND, "username");
        const char *password = MHD_lookup_connection_value(connection, MHD_POSTDATA_KIND, "password");
        if (username && password) {
            return do_login_function(connection, username, password);
        }
    }
    return send_html_response(connection, "Invalid request", MHD_HTTP_BAD_REQUEST);
}

int main() {
    struct MHD_Daemon *daemon;

    daemon = MHD_start_daemon(MHD_USE_INTERNAL_POLLING_THREAD, PORT, NULL, NULL,
                              &answer_to_connection, NULL, MHD_OPTION_END);
    if (NULL == daemon) {
        fprintf(stderr, "Failed to start the server.\n");
        return 1;
    }

    printf("Server running on port %d. Press enter to terminate.\n", PORT);
    getchar(); // Wait for user input to terminate the server

    MHD_stop_daemon(daemon);
    return 0;
}
