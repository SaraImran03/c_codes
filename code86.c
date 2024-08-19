#include <microhttpd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql/mysql.h>

#define PORT 8888

// MySQL database details
const char *server = "localhost";
const char *user = "your_username";
const char *password = "your_password";
const char *database = "weather_data";

// Function to send HTTP responses
int send_response(struct MHD_Connection *connection, const char *data, int status_code) {
    struct MHD_Response *response = MHD_create_response_from_buffer(strlen(data), (void *)data, MHD_RESPMEM_MUST_COPY);
    int ret = MHD_queue_response(connection, status_code, response);
    MHD_destroy_response(response);
    return ret;
}

// Connect to the database
MYSQL *connect_database() {
    MYSQL *conn = mysql_init(NULL);
    if (mysql_real_connect(conn, server, user, password, database, 0, NULL, 0) == NULL) {
        fprintf(stderr, "%s\n", mysql_error(conn));
        return NULL;
    }
    return conn;
}

// Handler for the API endpoint
int answer_to_connection(void *cls, struct MHD_Connection *connection,
                         const char *url, const char *method, const char *version,
                         const char *upload_data, size_t *upload_data_size, void **con_cls) {
    if (strcmp(url, "/api/temperature_for_location") != 0 || strcmp(method, "GET") != 0) {
        return MHD_NO;  // Only handle specific route and method
    }

    const char *latitude = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "latitude");
    const char *longitude = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "longitude");
    const char *year = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "year");
    const char *month = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "month");
    const char *day = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "day");
    const char *grib_file = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "grib_file");

    if (!latitude || !longitude || !year || !month || !day || !grib_file) {
        return send_response(connection, "Missing required parameters", MHD_HTTP_BAD_REQUEST);
    }

    // Perform database query
    MYSQL *conn = connect_database();
    if (!conn) {
        return send_response(connection, "Database connection failed", MHD_HTTP_INTERNAL_SERVER_ERROR);
    }

    char query[512];
    snprintf(query, sizeof(query), "SELECT MAX(temperature) FROM temperatures WHERE latitude=%s AND longitude=%s AND year=%s AND month=%s AND day=%s AND grib_file='%s'",
             latitude, longitude, year, month, day, grib_file);

    if (mysql_query(conn, query)) {
        mysql_close(conn);
        return send_response(connection, mysql_error(conn), MHD_HTTP_INTERNAL_SERVER_ERROR);
    }

    MYSQL_RES *result = mysql_store_result(conn);
    MYSQL_ROW row = mysql_fetch_row(result);
    char response[256];
    if (row && row[0]) {
        snprintf(response, sizeof(response), "{\"max_temperature\": %s}", row[0]);
    } else {
        strcpy(response, "No data found");
    }
    mysql_free_result(result);
    mysql_close(conn);

    return send_response(connection, response, MHD_HTTP_OK);
}

int main() {
    struct MHD_Daemon *daemon = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY, PORT, NULL, NULL,
                                                 &answer_to_connection, NULL, MHD_OPTION_END);
    if (daemon == NULL) {
        fprintf(stderr, "Failed to start the web server.\n");
        return EXIT_FAILURE;
    }

    printf("Server running on port %d. Press enter to terminate.\n", PORT);
    getchar();

    MHD_stop_daemon(daemon);
    return EXIT_SUCCESS;
}
