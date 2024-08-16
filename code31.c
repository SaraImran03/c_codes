#include <stdio.h>
#include <string.h>
#include <curl/curl.h>

// Forward declaration of functions
void redirect_url(const char* url);
void return_404(const char* url);

// Function to parse the URL and determine redirection
void redirect_url(const char* url) {
    CURLU *h;
    CURLUcode uc;
    char *host;
    
    h = curl_url(); // Initialize a CURLU handle
    if (!h) {
        return_404(url);
        return;
    }

    // Set the URL and parse it
    uc = curl_url_set(h, CURLUPART_URL, url, 0);
    if (uc) {
        curl_url_cleanup(h); // Clean up on failure
        return_404(url);
        return;
    }

    // Extract the hostname from the URL
    uc = curl_url_get(h, CURLUPART_HOST, &host, 0);
    if (uc) {
