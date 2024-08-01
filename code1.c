#include <stdio.h>
#include <string.h>

// Define a constant for the maximum buffer size
#define MAX_BUFFER_SIZE 256

// Function to copy bytes from source to destination
void copyBytes(unsigned char *dest, const unsigned char *src, size_t numBytes) {
    // Check if the number of bytes to copy exceeds the buffer capacity
    if (numBytes > MAX_BUFFER_SIZE) {
        printf("Error: Attempt to copy more bytes than the buffer can hold.\n");
        return;
    }

    // Use memcpy to copy the data
    memcpy(dest, src, numBytes);

    // Optionally, print a success message or the copied data
    printf("Bytes copied successfully.\n");
}

int main() {
    unsigned char dest[MAX_BUFFER_SIZE];  // Destination buffer
    unsigned char src[MAX_BUFFER_SIZE];   // Source buffer

    // Initialize source buffer with some data
    for (int i = 0; i < MAX_BUFFER_SIZE; i++) {
        src[i] = i;  // Fill with some data for demonstration
    }

    // Copy 100 bytes from src to dest
    copyBytes(dest, src, 100);

    // Optionally, print the first few copied bytes to verify
    printf("First 10 copied bytes: ");
    for (int i = 0; i < 10; i++) {
        printf("%d ", dest[i]);
    }
    printf("\n");

    return 0;
}
