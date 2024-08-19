#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

// Function to check if the file extension is safe
int file_is_safe_type(const char *filename) {
    const char *safe_extensions[] = { ".png", ".jpg", ".jpeg", ".gif", ".txt", NULL };
    const char *ext = strrchr(filename, '.'); // Find the last '.' in filename to get the extension

    if (!ext) {
        return 0; // No extension found, unsafe
    }

    for (int i = 0; safe_extensions[i] != NULL; i++) {
        if (strcmp(ext, safe_extensions[i]) == 0) {
            return 1; // Extension is safe
        }
    }

    return 0; // Extension is not in the list of safe extensions
}

// Function to upload a file
void upload_file(const char *source_path, const char *filename) {
    if (!file_is_safe_type(filename)) {
        printf("File type of %s is not allowed for upload.\n", filename);
        return;
    }

    // Create directory if it does not exist
    struct stat st = {0};
    if (stat("uploads", &st) == -1) {
        mkdir("uploads", 0700);
    }

    char file_path[512];
    snprintf(file_path, sizeof(file_path), "uploads/%s", filename); // Construct path to save file

    // Assuming the file is a simple text file for this example
    FILE *source = fopen(source_path, "rb");
    if (!source) {
        perror("Failed to open source file");
        return;
    }

    FILE *dest = fopen(file_path, "wb");
    if (!dest) {
        perror("Failed to open destination file");
        fclose(source);
        return;
    }

    // Copy file contents
    char buffer[1024];
    size_t bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), source)) > 0) {
        fwrite(buffer, 1, bytes, dest);
    }

    fclose(source);
    fclose(dest);

    printf("File %s has been uploaded successfully to %s.\n", filename, file_path);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <source_file> <filename>\n", argv[0]);
        return EXIT_FAILURE;
    }

    upload_file(argv[1], argv[2]);

    return EXIT_SUCCESS;
}
