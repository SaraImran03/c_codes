#include <stdio.h>
#include <string.h>

void copyBytes(char *dest, const char *src, size_t n) {
    // Ensure that we do not exceed the buffer limit of 256 bytes
    if (n > 256) n = 256;

    // Copy the bytes from source to destination
    memcpy(dest, src, n);
}

int main() {
    char dest[256];
    const char src[] = "Hello, this is a test message.";

    // Copy 24 bytes from src to dest
    copyBytes(dest, src, 24);

    // Print the copied message
    printf("Copied Message: %s\n", dest);

    return 0;
}

 
