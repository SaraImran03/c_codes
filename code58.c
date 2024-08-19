#include <microhttpd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PORT 8888

typedef struct {
    char username[256];
    char email[256];
    char password[256];
    int is_logged_in;
} User;

User user = {"user", "user@example.com", "password123", 0}; // Simulated user database

// Function to send HTML responses
int send_html_response(struct MHD_Connection *connection, const char *html, int status_code) {
    struct MHD_Response *response = MHD_create_response_from_buffer(strlen(html), (void*)html, MHD_RESPMEM_MUST_COPY);
    int ret = MHD_queue_response(connection, status_code, response);
    MHD_destroy_response(response);
    return ret;
}

// HTML form for login
const char *login_page =
    "<html><body>"
    "<h1>Login</h1>"
    "<form action='/login' method='post'>"
    "Username: <input type='text' name='username'><br>"
    "Password: <input type='password' name='password'><br>"
    "<input type='submit' value='Login'>"
    "</form>"
    "</body></html>";

// HTML form for changing email
const char *get_change_email_form() {
    static char form[1024];
    sprintf(form,
            "<html><body>"
            "<h1>Change Email</h1>"
            "<form action='/change_email' method='post'>"
            "Old Email: <input type='text' name='old_email'><br>"
            "New Email: <input type='text' name='new_email'><br>"
            "Confirm Password: <input type='password' name='password'><br>"
            "<input type='submit' value='Change Email'>"
            "</form>"
            "</body></html>");
    return form;
}

// Login handling function
int handle_login(const char *username, const char *password) {
    if (strcmp(username, user.username) == 0 && strcmp(password, user.password) == 0) {
        user.is_logged_in = 1;
        return 1;
    }
    return 0;
}

// Change email handling function
int handle_change_email(const char *old_email, const char *new_email, const char *password) {
    if (user.is_logged_in && strcmp(old_email, user.email) == 0 && strcmp(password, user.password) == 0) {
        strcpy(user.email, new_email);
        return 1;
    }
    return 0;
}

// Request handler for incoming connections
int answer_to_connection(void *cls, struct MHD_Connection *connection,
                         const char *url, const char *method,
                         const char *version, const char *upload_data,
                         size_t *upload_data_size, void **con_cls) {
    if (strcmp(method, "GET") == 0 && strcmp(url, "/login") == 0) {
        return send_html_response(connection, login_page, MHD_HTTP_OK);
    } else if (strcmp(method, "GET") == 0 && strcmp(url, "/change_email") == 0) {
        if (user.is_logged_in) {
            return send_html_response(connection, get_change_email_form(), MHD_HTTP_OK);
        } else {
            return send_html_response(connection, "Unauthorized access. Please login first.", MHD_HTTP_UNAUTHORIZED);
        }
    } else if (strcmp(method, "POST") == 0) {
        const char *username = MHD_lookup_connection_value(connection, MHD_POSTDATA_KIND, "username");
        const char *password = MHD_lookup_connection_value(connection, MHD_POSTDATA_KIND, "password");
        if (strcmp(url, "/login") == 0 && handle_login(username, password)) {
            return send_html_response(connection, "Login successful. <a href='/change_email'>Change Email</a>", MHD_HTTP_OK);
        } else if (strcmp(url, "/change_email") == 0) {
            const char *old_email = MHD_lookup_connection_value(connection, MHD_POSTDATA_KIND, "old_email");
            const char *new_email = MHD_lookup_connection_value(connection, MHD_POSTDATA_KIND, "new_email");
            if (handle_change_email(old_email, new_email, password)) {
                return send_html_response(connection, "Email changed successfully.", MHD_HTTP_OK);
            } else {
                return send_html_response(connection, "Failed to change email. Invalid details provided.", MHD_HTTP_BAD_REQUEST);
            }
        }
    }

    return send_html_response(connection, "Page not found.", MHD_HTTP_NOT_FOUND);
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
