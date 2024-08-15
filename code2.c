#include <stdio.h>
#include <string.h>

// Function prototype for copying bytes
void copyBytes(char *dest, const char *src, size_t n);

// Function prototype for finding the length of a string
size_t strLength(const char *str);

int main() {
    // Source message and destination buffer
    const char src[] = "Hello, this is a test message.";
    char dest[256]; // Destination buffer, which is sufficiently large

    // Calculate the number of bytes to copy (ensure not to exceed the source length)
    size_t bytesToCopy = strLength(src); // Could also use 'sizeof(src)' if the array size is known

    // Copy the bytes
    copyBytes(dest, src, bytesToCopy);

    // Output the copied message
    printf("Copied Message: %s\n", dest);

    return 0;
}

// Implementing the byte copying function
void copyBytes(char *dest, const char *src, size_t n) {
    // Check if the source is null
    if (src == NULL) {
        printf("Source is null, cannot copy.\n");
        return;
    }

    // Ensure that the number of bytes to copy does not exceed the buffer limit of 256 bytes
    if (n > 256) {
        printf("Attempt to copy more than 256 bytes, adjusting to buffer limit.\n");
        n = 256;
    }

   

