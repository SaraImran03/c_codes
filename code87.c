#include <microhttpd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql/mysql.h>

#define PORT 8888

// Database connection parameters
const char *db_host = "localhost";
const char *db_user = "username";
const char *db_password = "password";
const char *db_name = "weather_database";

// Connect to the database
MYSQL* connect_database() {
    MYSQL *conn = mysql_init(NULL);
    if (conn == NULL) {
        fprintf(stderr, "MySQL initialization failed: %s\n", mysql_error(conn));
        return NULL;
    }

    if (!mysql_real_connect(conn, db_host, db_user, db_password, db_name, 0, NULL, 0)) {
        fprintf(stderr, "MySQL connection error: %s\n", mysql_error(conn));
        mysql_close(conn);
        return NULL;
    }

    return conn;
}

// Query the database for temperature
double query_temperature(MYSQL *conn, const char *latitude, const char *longitude, const char *date) {
    char query[512];
    snprintf(query, sizeof(query), "SELECT temperature FROM temperature_data WHERE latitude = '%s' AND longitude = '%s' AND date = '%s'", latitude, longitude, date);

    if (mysql_query(conn, query)) {
        fprintf(stderr, "MySQL query error: %s\n", mysql_error(conn));
        return -1;
    }

    MYSQL_RES *result = mysql_store_result(conn);
    MYSQL_ROW row = mysql_fetch_row(result);

    double temperature = -1;
    if (row && row[0]) {
        temperature = atof(row[0]);
    }

    mysql_free_result(result);
    return temperature;
}

// Handle incoming HTTP requests
static int answer_to_connection(void *cls, struct MHD_Connection *connection, const char *url,
                                const char *method, const char *version,
                                const char *upload_data, size_t *upload_data_size, void **con_cls) {
    if (strcmp(url, "/api/temperature_for_location") != 0 || strcmp(method, "GET") != 0) {
        return MHD_NO; // Only accept GET requests to the specified URL
    }

    const char *latitude = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "latitude");
    const char *longitude = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "longitude");
    const char *date = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "date");

    if (!latitude || !longitude || !date) {
        const char *error = "Missing required parameters (latitude, longitude, date)";
        struct MHD_Response *response = MHD_create_response_from_buffer(strlen(error), (void *)error, MHD_RESPMEM_PERSISTENT);
        return MHD_queue_response(connection, MHD_HTTP_BAD_REQUEST, response);
    }

    MYSQL *conn = connect_database();
    if (!conn) {
        const char *error = "Failed to connect to database";
        struct MHD_Response *response = MHD_create_response_from_buffer(strlen(error), (void *)error, MHD_RESPMEM_PERSISTENT);
        return MHD_queue_response(connection, MHD_HTTP_INTERNAL_SERVER_ERROR, response);
    }

    double temperature = query_temperature(conn, latitude, longitude, date);
    mysql_close(conn);

    if (temperature == -1) {
        const char *error = "Temperature data not found";
        struct MHD_Response *response = MHD_create_response_from_buffer(strlen(error), (void *)error, MHD_RESPMEM_PERSISTENT);
        return MHD_queue_response(connection, MHD_HTTP_NOT_FOUND, response);
    }

    char result[64];
    snprintf(result, sizeof(result), "Temperature: %.2f degrees", temperature);
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
    getchar();

    MHD_stop_daemon(daemon);
    return EXIT_SUCCESS;
}
