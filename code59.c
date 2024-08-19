#include <stdio.h>

void fillBuffer(int buffer_size) {
    // Allocate buffer on the stack
    char buffer[buffer_size];

    // Fill the buffer with 'A'
    for (int i = 0; i < buffer_size; i++) {
        buffer[i] = 'A';
    }

    // Print the buffer content
    printf("Buffer content: ");
    for (int i = 0; i < buffer_size; i++) {
        printf("%c", buffer[i]);
    }
    printf("\n");
}

int main() {
    int buffer_size;

    // Ask the user for the buffer size
    printf("Enter buffer size: ");
    scanf("%d", &buffer_size);

    // Call function to allocate and fill the buffer
    fillBuffer(buffer_size);

    return 0;
}
