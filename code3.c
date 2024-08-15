#include <stdio.h>
#include <string.h>

// Function prototype for copying bytes
void copyBytes(char *dest, const char *src, size_t n);

// Function prototype for finding the length of a string
size_t strLength(const char *str);

int main() {
    char dest[256];  // Destination buffer
    const char src[] = "Hello";  // Source string

    // Calculate the number of bytes to copy
    size_t bytesToCopy = strLength(src);  // Using string length to determine bytes to copy

    // Copy the string to the buffer
    copyBytes(dest, src, bytesToCopy);

    // Output the copied string
    printf("Copied String: %s\n", dest);

    return 0;
}

// Implementing the byte copying function
void copyBytes(char *dest, const char *src, size_t n) {
    // Ensure that we do not exceed the buffer limit of 256 bytes
    if (n > 256) {
        printf("Buffer overflow prevented, adjusting to buffer limit.\n");
        n = 256;
    }

    // Copy bytes from source to destination using a loop
    for (size_t i = 0; i < n; i++) {
        dest[i] = src[i];
    }
    dest[n] = '\0'; // Null-terminate the destination buffer
}

// Implementing the string length function
size_t strLength(const char *str) {
    size_t length = 0;
    while (str[length] != '\0') {
        length++;
    }
    return length;
}
