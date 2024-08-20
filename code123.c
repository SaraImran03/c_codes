#include <stdio.h>
#include <string.h>
#include <ctype.h>  // For isspace()

int main(int argc, char *argv[]) {
    // Check to ensure exactly one additional command line argument is provided
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <string>\n", argv[0]);
        return 1;
    }

    char *str = argv[1];  // Point to the argument string

    // Trim trailing whitespace
    int len = strlen(str);
    while (len > 0 && isspace((unsigned char)str[len - 1])) {
        len--;
    }
    
    // Trim trailing newlines (often included with whitespace, but handled separately for clarity)
    while (len > 0 && str[len - 1] == '\n') {
        len--;
    }

    str[len] = '\0';  // Null-terminate the modified string

    printf("Trimmed string: '%s'\n", str);

    return 0;
}
