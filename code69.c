#include <stdio.h>
#include <stdlib.h>
#include "cencode.h"  // Include the header for the libb64 encoder

// Function to read the entire file into memory
unsigned char *read_file(const char *filename, size_t *length) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Failed to open file");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    *length = ftell(file);
    fseek(file, 0, SEEK_SET);

    unsigned char *data = malloc(*length);
    if (!data) {
        fprintf(stderr, "Memory allocation failed\n");
        fclose(file);
        return NULL;
    }

    fread(data, 1, *length, file);
    fclose(file);
    return data;
}

// Function to encode data to Base64 and write it to a file
int encode_to_base64_and_save(const unsigned char *input, size_t input_length, const char *output_filename) {
    FILE *output_file = fopen(output_filename, "w");
    if (!output_file) {
        perror("Failed to open output file");
        return 0;
    }

    base64_encodestate state;
    base64_init_encodestate(&state);

    char *encoded_data = malloc(2 * input_length);  // Enough memory for encoding
    if (!encoded_data) {
        fclose(output_file);
        return 0;
    }

    int encoded_length = base64_encode_block((char *)input, input_length, encoded_data, &state);
    encoded_length += base64_encode_blockend(encoded_data + encoded_length, &state);

    fwrite(encoded_data, 1, encoded_length, output_file);
    fclose(output_file);
    free(encoded_data);
    return 1;
}

// Main function that simulates the upload process
int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input image file> <output base64 file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    size_t input_length;
    unsigned char *file_data = read_file(argv[1], &input_length);
    if (!file_data) {
        return EXIT_FAILURE;
    }

    int result = encode_to_base64_and_save(file_data, input_length, argv[2]);
    free(file_data);

    if (result) {
        printf("Image was successfully encoded and saved.\n");
    } else {
        fprintf(stderr, "Failed to encode and save the image.\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
