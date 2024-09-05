#include <stdio.h>
#include <string.h>

void vulnerable_function(char *str) {
    char buffer[10];
    strcpy(buffer, str); // This is vulnerable to buffer overflow
    printf("Buffer contains: %s\n", buffer);
}

int main() {
    char large_input[256];
    memset(large_input, 'A', sizeof(large_input) - 1);
    large_input[255] = '\0'; // Ensure null-terminated string

    printf("Simulating buffer overflow...\n");
    vulnerable_function(large_input); // This will overflow buffer

    return 0;
}
