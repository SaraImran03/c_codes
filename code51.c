#include <archive.h>
#include <archive_entry.h>
#include <stdio.h>
#include <stdlib.h>

// Function to extract tar files using libarchive
int extract_tar_file(const char *path, const char *out_dir) {
    struct archive *a;
    struct archive *a_out;
    struct archive_entry *entry;
    int r;

    // Create and configure the archive for reading
    a = archive_read_new();
    archive_read_support_format_tar(a);
    archive_read_support_filter_all(a);

    // Open the tar file
    if (archive_read_open_filename(a, path, 10240) != ARCHIVE_OK) {
        fprintf(stderr, "Failed to open tar file: %s\n", archive_error_string(a));
        archive_read_free(a);
        return 1;
    }

    // Create and configure the archive for extracting
    a_out = archive_write_disk_new();
    archive_write_disk_set_options(a_out, ARCHIVE_EXTRACT_TIME | ARCHIVE_EXTRACT_PERM | ARCHIVE_EXTRACT_ACL | ARCHIVE_EXTRACT_FFLAGS);
    archive_write_disk_set_standard_lookup(a_out);

    // Read each entry from the tar file
    while ((r = archive_read_next_header(a, &entry)) == ARCHIVE_OK) {
        printf("Extracting: %s\n", archive_entry_pathname(entry));
        archive_entry_set_pathname(entry, archive_entry_pathname(entry));  // Set extract path

        if (archive_write_header(a_out, entry) == ARCHIVE_OK) {
            copy_data(a, a_out);
        }

        // Complete the extraction
        archive_write_finish_entry(a_out);
    }

    // Clean up
    archive_read_close(a);
    archive_read_free(a);
    archive_write_close(a_out);
    archive_write_free(a_out);

    if (r != ARCHIVE_EOF) {
        fprintf(stderr, "Failed to read all entries: %s\n", archive_error_string(a));
        return 1;
    }

    return 0;
}

// Helper function to copy data from the archive to the disk
int copy_data(struct archive *ar, struct archive *aw) {
    const void *buff;
    size_t size;
    la_int64_t offset;
    int r;

    while ((r = archive_read_data_block(ar, &buff, &size, &offset)) == ARCHIVE_OK) {
        if (archive_write_data_block(aw, buff, size, offset) != ARCHIVE_OK) {
            fprintf(stderr, "Write error: %s\n", archive_error_string(aw));
            return ARCHIVE_FATAL;
        }
    }
    return r;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <tarfile> <output_dir>\n", argv[0]);
        return 1;
    }

    const char *tar_path = argv[1];
    const char *output_dir = argv[2];

    if (extract_tar_file(tar_path, output_dir) != 0) {
        fprintf(stderr, "Extraction failed\n");
        return EXIT_FAILURE;
    }

    printf("Extraction complete\n");
    return EXIT_SUCCESS;
}
