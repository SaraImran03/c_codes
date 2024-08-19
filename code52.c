#include <microhttpd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PORT 8888

// Session structure to hold session data
typedef struct {
    char username[256];
    int is_logged_in;
} Session;

Session session;

// HTML content for different pages
const char *login_form =
    "<html><body>"
    "<form action='/login' method='post'>"
    "Username: <input type='text' name='username'><br>"
    "Password: <input type='password' name='password'><br>"
    "<input type='submit' value='Login'>"
    "</form>"
    "</body></html>";

const char *index_page =
    "<html><body>"
    "<h1>Welcome to the Home Page</h1>"
    "<a href='/login'>Login</a>"
    "</body></html>";

const char *secret_page =
    "<html><body>"
    "<h1>Secret settings</h1>"
    "</body></html>";

// Helper function to send HTML response
int send_html_response(struct MHD_Connection *connection, const char *page) {
    struct MHD_Response *response = MHD_create_response_from_buffer(strlen(page), (void *)page, MHD_RESPMEM_PERSISTENT);
    int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    MHD_destroy_response(response);
    return ret;
}

// Function to handle login
int do_login(struct MHD_Connection *connection, const char *username, const char *password) {
    // Simple check for username and password
    if (strcmp(username, "admin") == 0 && strcmp(password, "admin123") == 0) {
        strcpy(session.username, username);
        session.is_logged_in = 1;
        return send_html_response(connection, secret_page);
    } else {
        return send_html_response(connection, login_form);
    }
}

// Function to handle different routes
int answer_to_connection(void *cls, struct MHD_Connection *connection,
                         const char *url, const char *method,
                         const char *version, const char *upload_data,
                         size_t *upload_data_size, void **con_cls) {
    if (strcmp(method, "GET") == 0) {
        if (strcmp(url, "/login") == 0)
            return send_html_response(connection, login_form);
        else if (strcmp(url, "/secret") == 0 && session.is_logged_in)
            return send_html_response(connection, secret_page);
        else
            return send_html_response(connection, index_page);
    } else if (strcmp(method, "POST") == 0 && strcmp(url, "/login") == 0) {
        const char *username = MHD_lookup_connection_value(connection, MHD_POSTDATA_KIND, "username");
        const char *password = MHD_lookup_connection_value(connection, MHD_POSTDATA_KIND, "password");
        return do_login(connection, username, password);
    }

    return MHD_NO; // Invalid request
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
