#include <microhttpd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PORT 8888

typedef struct {
    int is_logged_in;
    int is_admin;
} UserSession;

UserSession session;

// HTML content for different pages
const char *login_form =
    "<html><body>"
    "<form action='/do_login' method='post'>"
    "Username: <input type='text' name='username'><br>"
    "Password: <input type='password' name='password'><br>"
    "<input type='submit' value='Login'>"
    "</form>"
    "</body></html>";

const char *index_page =
    "<html><body>"
    "<h1>Welcome to the Index Page</h1>"
    "<a href='/login'>Login</a>"
    "</body></html>";

const char *user_page =
    "<html><body>"
    "<h1>User Page</h1>"
    "<p>Welcome, User!</p>"
    "</body></html>";

const char *admin_page =
    "<html><body>"
    "<h1>Admin Page</h1>"
    "<p>Welcome, Admin!</p>"
    "</body></html>";

// Send HTML response
int send_html_response(struct MHD_Connection *connection, const char *html) {
    struct MHD_Response *response = MHD_create_response_from_buffer(strlen(html), (void *)html, MHD_RESPMEM_MUST_COPY);
    int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    MHD_destroy_response(response);
    return ret;
}

// Handlers for GET and POST requests
int handle_get_request(const char *url, struct MHD_Connection *connection) {
    if (strcmp(url, "/login") == 0)
        return send_html_response(connection, login_form);
    else if (strcmp(url, "/") == 0)
        return send_html_response(connection, index_page);
    else if (strcmp(url, "/user_page") == 0 && session.is_logged_in && !session.is_admin)
        return send_html_response(connection, user_page);
    else if (strcmp(url, "/admin_page") == 0 && session.is_logged_in && session.is_admin)
        return send_html_response(connection, admin_page);

    return MHD_NO;  // 404 Not Found
}

int handle_post_request(const char *url, struct MHD_Connection *connection) {
    if (strcmp(url, "/do_login") == 0) {
        const char *username = MHD_lookup_connection_value(connection, MHD_POSTDATA_KIND, "username");
        const char *password = MHD_lookup_connection_value(connection, MHD_POSTDATA_KIND, "password");
        if (strcmp(username, "user") == 0 && strcmp(password, "password") == 0) {
            session.is_logged_in = 1;
            session.is_admin = 0;
            return send_html_response(connection, user_page);
        } else if (strcmp(username, "admin") == 0 && strcmp(password, "admin") == 0) {
            session.is_logged_in = 1;
            session.is_admin = 1;
            return send_html_response(connection, admin_page);
        }
    }

    return send_html_response(connection, login_form);  // Show login form again on failure
}

int answer_to_connection(void *cls, struct MHD_Connection *connection,
                         const char *url, const char *method,
                         const char *version, const char *upload_data,
                         size_t *upload_data_size, void **con_cls) {
    if (strcmp(method, "GET") == 0)
        return handle_get_request(url, connection);
    else if (strcmp(method, "POST") == 0)
        return handle_post_request(url, connection);

    return MHD_NO;  // 404 Not Found
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
    getchar(); // Wait for user input to terminate server

    MHD_stop_daemon(daemon);
    return 0;
}
