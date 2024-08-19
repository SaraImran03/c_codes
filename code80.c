#include <stdio.h>
#include <string.h>

#define BUFFER_SIZE 1024  // Buffer size including room for null terminator

int main() {
    char buffer[BUFFER_SIZE];  // Declare a buffer for input
    printf("Enter a string (up to 1023 characters): ");

    // Read in the string with scanf using a format specifier that limits the input length
    if (scanf("%1023[^\n]%*c", buffer) == 1) {  // Read up to 1023 characters until a newline, consume the newline
        // Output the number of characters read (not counting the null terminator)
        printf("You entered: '%s'\n", buffer);
        printf("Number of characters read: %ld\n", strlen(buffer));
    } else {
        printf("Failed to read input or no input provided.\n");
    }

    return 0;
}
