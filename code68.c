#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql/mysql.h>
#include "b64/cencode.h"

// Database parameters
#define DB_HOST "localhost"
#define DB_USER "user"
#define DB_PASS "password"
#define DB_NAME "dbname"

// Initialize database connection
MYSQL* init_db() {
    MYSQL *conn = mysql_init(NULL);
    if (!conn) {
        fprintf(stderr, "MySQL initialization failed\n");
        return NULL;
    }
    if (!mysql_real_connect(conn, DB_HOST, DB_USER, DB_PASS, DB_NAME, 0, NULL, 0)) {
        fprintf(stderr, "MySQL connection failed: %s\n", mysql_error(conn));
        return NULL;
    }
    return conn;
}

// Function to convert image data to Base64 string
char* base64_encode(const unsigned char *data, size_t input_length, size_t *output_length) {
    *output_length = 4 * ((input_length + 2) / 3);

    char *encoded_data = malloc(*output_length);
    if (encoded_data == NULL) return NULL;

    base64_encodestate state;
    base64_init_encodestate(&state);

    int len = base64_encode_block((const char *)data, input_length, encoded_data, &state);
    len += base64_encode_blockend(encoded_data + len, &state);

    return encoded_data;
}

// Function to read image and encode it
char* read_and_encode_image(const char *filename, size_t *output_length) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Failed to open file");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long input_length = ftell(file);
    rewind(file);

    unsigned char *data = malloc(input_length);
    if (data == NULL) {
        fclose(file);
        return NULL;
    }

    fread(data, 1, input_length, file);
    fclose(file);

    char *encoded_data = base64_encode(data, input_length, output_length);
    free(data);

    return encoded_data;
}

// Function to insert image data into MySQL database
int insert_image_data(MYSQL *conn, const char *image_name, const char *base64_data) {
    char *query = malloc(strlen(image_name) + strlen(base64_data) + 100);
    sprintf(query, "INSERT INTO images (name, data) VALUES ('%s', '%s')", image_name, base64_data);

    if (mysql_query(conn, query)) {
        fprintf(stderr, "MySQL query error: %s\n", mysql_error(conn));
        free(query);
        return 0;
    }

    free(query);
    return 1;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <image file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    MYSQL *conn = init_db();
    if (!conn) return EXIT_FAILURE;

    size_t output_length;
    char *encoded_image = read_and_encode_image(argv[1], &output_length);
    if (!encoded_image) {
        mysql_close(conn);
        return EXIT_FAILURE;
    }

    if (!insert_image_data(conn, argv[1], encoded_image)) {
        free(encoded_image);
        mysql_close(conn);
        return EXIT_FAILURE;
    }

    free(encoded_image);
    printf("Image uploaded and stored in database successfully.\n");
    mysql_close(conn);
    return EXIT_SUCCESS;
}
