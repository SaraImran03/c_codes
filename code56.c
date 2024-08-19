#include <microhttpd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PORT 8888

// Simulated user database
typedef struct {
    char username[256];
    char email[256];
    char password[256];
    int is_logged_in;
} User;

User user = {"user", "user@example.com", "password123", 0};

// Function to send HTML response
int send_html_response(struct MHD_Connection *connection, const char *html, int status_code) {
    struct MHD_Response *response = MHD_create_response_from_buffer(strlen(html), (void*)html, MHD_RESPMEM_MUST_COPY);
    int ret = MHD_queue_response(connection, status_code, response);
    MHD_destroy_response(response);
    return ret;
}

// HTML form for changing email
const char *get_email_change_form(const char *old_email) {
    static char form[1024];
    sprintf(form,
            "<html><body>"
            "<h1>Change Email</h1>"
            "<form action='/change_email' method='post'>"
            "Old Email: <input type='text' name='old_email' value='%s' readonly><br>"
            "New Email: <input type='text' name='new_email'><br>"
            "Confirm Password: <input type='password' name='password'><br>"
            "<input type='submit' value='Update Email'>"
            "</form>"
            "</body></html>", old_email);
    return form;
}

// Handle GET requests
int handle_get_request(const char *url, struct MHD_Connection *connection) {
    if (strcmp(url, "/login") == 0) {
        return send_html_response(connection, "Login Required", MHD_HTTP_UNAUTHORIZED);
    } else if (strcmp(url, "/change_email") == 0) {
        if (user.is_logged_in) {
            return send_html_response(connection, get_email_change_form(user.email), MHD_HTTP_OK);
        } else {
            return send_html_response(connection, "Login Required", MHD_HTTP_UNAUTHORIZED);
        }
    }
    return MHD_NO; // 404 Not Found
}

// Handle POST requests
int handle_post_request(const char *url, struct MHD_Connection *connection) {
    if (strcmp(url, "/change_email") == 0) {
        const char *old_email = MHD_lookup_connection_value(connection, MHD_POSTDATA_KIND, "old_email");
        const char *new_email = MHD_lookup_connection_value(connection, MHD_POSTDATA_KIND, "new_email");
        const char *password = MHD_lookup_connection_value(connection, MHD_POSTDATA_KIND, "password");

        if (!user.is_logged_in) {
            return send_html_response(connection, "Login Required", MHD_HTTP_UNAUTHORIZED);
        } else if (strcmp(old_email, user.email) != 0) {
            return send_html_response(connection, "Old email does not match.", MHD_HTTP_BAD_REQUEST);
        } else if (strcmp(password, user.password) != 0) {
            return send_html_response(connection, "Incorrect password.", MHD_HTTP_BAD_REQUEST);
        } else if (strcmp(new_email, old_email) == 0) {
            return send_html_response(connection, "New email cannot be the same as old email.", MHD_HTTP_BAD_REQUEST);
        } else {
            strcpy(user.email, new_email); // Update email in the "database"
            return send_html_response(connection, "Email updated successfully.", MHD_HTTP_OK);
        }
    }
    return MHD_NO; // 404 Not Found
}

// Main connection handler
int answer_to_connection(void *cls, struct MHD_Connection *connection,
                         const char *url, const char *method,
                         const char *version, const char *upload_data,
                         size_t *upload_data_size, void **con_cls) {
    if (strcmp(method, "GET") == 0) {
        return handle_get_request(url, connection);
    } else if (strcmp(method, "POST") == 0) {
        return handle_post_request(url, connection);
    }
    return MHD_NO; // 404 Not Found
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
