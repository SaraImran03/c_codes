#include <zip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

// Function to create directory if it does not exist
int create_directory(const char *dir) {
    struct stat st = {0};
    if (stat(dir, &st) == -1) {
        if (mkdir(dir, 0755) != 0) {
            perror("Failed to create directory");
            return -1;
        }
    }
    return 0;
}

// Function to extract a ZIP file
int extract_zip_file(const char *archive_path, const char *extract_path) {
    int err = 0;
    struct zip *za = zip_open(archive_path, 0, &err);
    struct zip_file *zf;
    struct zip_stat sb;
    char buf[100];
    FILE *f;
    int i, len;
    
    if (za == NULL) {
        zip_error_to_str(buf, sizeof(buf), err, errno);
        fprintf(stderr, "Error opening zip file '%s': %s\n", archive_path, buf);
        return -1;
    }

    // Create directory for extraction
    if (create_directory(extract_path) != 0) {
        zip_close(za);
        return -1;
    }

    // Loop through all files in the archive
    for (i = 0; i < zip_get_num_entries(za, 0); i++) {
        if (zip_stat_index(za, i, 0, &sb) == 0) {
            printf("Extracting %s\n", sb.name);
            zf = zip_fopen_index(za, i, 0);
            if (!zf) {
                fprintf(stderr, "Error opening file in zip: %s\n", sb.name);
                continue;
            }

            // Create a full path for files to be extracted
            snprintf(buf, sizeof(buf), "%s/%s", extract_path, sb.name);
            f = fopen(buf, "wb");
            if (!f) {
                fprintf(stderr, "Error opening file on disk: %s\n", buf);
                zip_fclose(zf);
                continue;
            }

            // Read from zip and write to file
            while ((len = zip_fread(zf, buf, sizeof(buf))) > 0) {
                fwrite(buf, 1, len, f);
            }
            fclose(f);
            zip_fclose(zf);
        }
    }

    zip_close(za);
    return 0;
}

int main(int argc, char **argv) {
    const char *archive_path = "archive.zip";  // Path to the ZIP file
    const char *extract_path = "/tmp/unpack"; // Target path for extraction

    if (extract_zip_file(archive_path, extract_path) != 0) {
        fprintf(stderr, "Failed to extract zip file.\n");
        return EXIT_FAILURE;
    }

    printf("Files extracted successfully.\n");
    return EXIT_SUCCESS;
}
