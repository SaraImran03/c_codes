#include <microhttpd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <json-c/json.h>
#include <mysql/mysql.h>

#define PORT 8888

// Database connection parameters
const char* db_host = "localhost";
const char* db_user = "your_username";
const char* db_pass = "your_password";
const char* db_name = "Sensor";

// Establish database connection
MYSQL* connect_database() {
    MYSQL *conn = mysql_init(NULL);
    if (conn == NULL) {
        fprintf(stderr, "MySQL initialization failed: %s\n", mysql_error(conn));
        return NULL;
    }

    if (mysql_real_connect(conn, db_host, db_user, db_pass, db_name, 0, NULL, 0) == NULL) {
        fprintf(stderr, "MySQL connection failed: %s\n", mysql_error(conn));
        mysql_close(conn);
        return NULL;
    }

    return conn;
}

// Fetch temperature from the database
double get_temperature(MYSQL *conn, double latitude, double longitude, const char *date) {
    char query[512];
    snprintf(query, sizeof(query), "SELECT temperature FROM temperature_data WHERE latitude = %f AND longitude = %f AND date = '%s'", latitude, longitude, date);

    if (mysql_query(conn, query)) {
        fprintf(stderr, "MySQL query error: %s\n", mysql_error(conn));
        return -1;
    }

    MYSQL_RES *result = mysql_store_result(conn);
    if (result == NULL) {
        fprintf(stderr, "MySQL store result error: %s\n", mysql_error(conn));
        return -1;
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    double temperature = row ? atof(row[0]) : -1;
    mysql_free_result(result);

    return temperature;
}

// HTTP response utility function
int send_response(struct MHD_Connection *connection, const char *data, int status_code) {
    struct MHD_Response *response = MHD_create_response_from_buffer(strlen(data), (void*)data, MHD_RESPMEM_MUST_COPY);
    int ret = MHD_queue_response(connection, status_code, response);
    MHD_destroy_response(response);
    return ret;
}

// Request handler
int answer_to_connection(void *cls, struct MHD_Connection *connection, 
                         const char *url, const char *method, 
                         const char *version, const char *upload_data, 
                         size_t *upload_data_size, void **con_cls) {
    if (strcmp(url, "/api/temperature_for_location") != 0)
        return MHD_NO; // Only handle specific route

    if (strcmp(method, "POST") == 0 && *upload_data_size > 0) {
        struct json_object *parsed_json;
        struct json_object *latitude, *longitude, *date;

        parsed_json = json_tokener_parse(upload_data);
        if (!json_object_object_get_ex(parsed_json, "latitude", &latitude) ||
            !json_object_object_get_ex(parsed_json, "longitude", &longitude) ||
            !json_object_object_get_ex(parsed_json, "date", &date)) {
            json_object_put(parsed_json);
            return send_response(connection, "Invalid JSON payload", MHD_HTTP_BAD_REQUEST);
        }

        MYSQL *conn = connect_database();
        if (!conn) {
            json_object_put(parsed_json);
            return send_response(connection, "Database connection failed", MHD_HTTP_INTERNAL_SERVER_ERROR);
        }

        double temp = get_temperature(conn, json_object_get_double(latitude), json_object_get_double(longitude), json_object_get_string(date));
        mysql_close(conn);
        json_object_put(parsed_json);

        char response[256];
        snprintf(response, sizeof(response), "{\"temperature\": %.2f}", temp);
        return send_response(connection, response, MHD_HTTP_OK);
    }

    return MHD_NO;
}

int main() {
    struct MHD_Daemon *daemon = MHD_start_daemon(MHD_USE_INTERNAL_POLLING_THREAD, PORT, NULL, NULL, 
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
