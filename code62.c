#include <stdio.h>
#include <stdlib.h>

// Function to compute a simple checksum
unsigned int compute_checksum(const char *buffer, size_t size) {
    unsigned int checksum = 0;
    for (size_t i = 0; i < size; ++i) {
        checksum += (unsigned char)buffer[i];
    }
    return checksum;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input file> <output file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *input_filename = argv[1];
    const char *output_filename = argv[2];

    // Open the input file for reading
    FILE *input_file = fopen(input_filename, "rb");
    if (!input_file) {
        perror("Failed to open input file");
        return EXIT_FAILURE;
    }

    // Seek to the end to find the file size
    fseek(input_file, 0, SEEK_END);
    long file_size = ftell(input_file);
    rewind(input_file);

    // Allocate buffer to hold file contents
    char *buffer = malloc(file_size);
    if (!buffer) {
        fclose(input_file);
        fprintf(stderr, "Memory allocation failed\n");
        return EXIT_FAILURE;
    }

    // Read the file into the buffer
    size_t bytes_read = fread(buffer, 1, file_size, input_file);
    fclose(input_file);  // Close the input file as soon as it's no longer needed

    // Compute the checksum of the buffer
    unsigned int checksum = compute_checksum(buffer, bytes_read);

    // Determine if the buffer is "empty"
    if (checksum == 0 && bytes_read > 0) {
        printf("The buffer is effectively empty (all zeros).\n");
    }

    // Open the output file for writing
    FILE *output_file = fopen(output_filename, "w");
    if (!output_file) {
        free(buffer);
        perror("Failed to open output file");
        return EXIT_FAILURE;
    }

    // Write the number of bytes read and the checksum to the output file
    fprintf(output_file, "Bytes read: %zu\nChecksum: %u\n", bytes_read, checksum);
    fclose(output_file);  // Close the output file

    // Clean up and exit
    free(buffer);
    return EXIT_SUCCESS;
}
