#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pcre.h>

// Global compiled regular expression object
pcre *re;

// Function to initialize the regex
void init_regex() {
    const char *error;
    int erroffset;

    // Regular expression pattern to match a specific type of URL
    char *pattern = "https?://(?:www\\.)?example\\.com/.*";

    // Compile the regular expression
    re = pcre_compile(pattern, 0, &error, &erroffset, NULL);
    if (re == NULL) {
        fprintf(stderr, "PCRE compilation failed at offset %d: %s\n", erroffset, error);
        exit(1);
    }
}

// Function to check if the URL matches the compiled regex
const char *match_url(const char *url) {
    int rc;
    int ovector[30]; // Output vector for substring information

    rc = pcre_exec(re, NULL, url, strlen(url), 0, 0, ovector, 30);
    if (rc < 0) {
        // No match found
        return "https://defaulturl.com";
    }

    // If match
    return url;
}

int main() {
    // Initialize the regular expression
    init_regex();

    // Target URL to check
    const char *target_url = "https://www.example.com/page";
    
    // Get the result, either the matching URL or the default
    const char *result_url = match_url(target_url);

    // Print the result
    printf("Redirect URL: %s\n", result_url);

    // Free the compiled regular expression
    pcre_free(re);

    return 0;
}
