#include <microhttpd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pcre.h>

#define PORT 8888

// Regular expression to check if the URL is from "example.com"
const char *domain_pattern = "https?://(?:.*\\.)?example\\.com";

// Function to validate the URL
int validate_url(const char *url) {
    pcre *re;
    const char *error;
    int erroffset;
    int rc;
    int ovector[30];

    // Compile the regular expression
    re = pcre_comp
