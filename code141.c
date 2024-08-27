#include <kore/kore.h>
#include <kore/http.h>

// Function prototypes
int page_home(struct http_request *);
const char *get_server_name(void);
const char *get_database_name(void);
const char *get_database_user(void);
const char *get_database_password(void);

// Initialize configuration details (normally, you would fetch these from a secure source)
static const char *server_name = "localhost";
static const char *database_name = "mydatabase";
static const char *database_user = "dbuser";
static const char *database_password = "dbpassword";

int page_home(struct http_request *req) {
    kore_response_printf(req, HTTP_STATUS_200, 
        "Server: %s\nDatabase: %s\nUser: %s\nPassword: %s\n",
        get_server_name(), get_database_name(), get_database_user(), get_database_password());
    return (KORE_RESULT_OK);
}

const char *get_server_name() {
    return server_name;
}

const char *get_database_name() {
    return database_name;
}

const char *get_database_user() {
    return database_user;
}

const char *get_database_password() {
    return database_password;
}

int main() {
    // Kore web server configuration
    kore_log_register("db_app", KORE_LOG_ERR, KORE_NO_LOGGING);
    kore_module_load(NULL, 0, NULL);
    kore_server_start();
    return 0;
}
