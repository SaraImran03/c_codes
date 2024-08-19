#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define BUFFER_SIZE 100  // Sufficient size to hold the concatenated string

int main() {
    float numbers[3];  // Array to store three random floats
    char str_a[30], str_b[30], str_c[30];  // String buffers for each float
    char combined[BUFFER_SIZE];  // Buffer to store the concatenated string
    FILE *fp;

    // Seed the random number generator
    srand(time(NULL));

    // Generate three random floats and convert each to a string
    for (int i = 0; i < 3; i++) {
        numbers[i] = (float)rand() / RAND_MAX * 100.0;  // Generate random float between 0 and 100
    }
    sprintf(str_a, "%f", numbers[0]);
    sprintf(str_b, "%f", numbers[1]);
    sprintf(str_c, "%f", numbers[2]);

    // Concatenate all strings into one
    snprintf(combined, sizeof(combined), "%s, %s, %s", str_a, str_b, str_c);

    // Open a file to write
    fp = fopen("output.txt", "w");
    if (fp == NULL) {
        perror("Unable to open file");
        return 1;
    }

    // Write the concatenated string to the file
    fprintf(fp, "%s\n", combined);
    printf("Written to file: %s\n", combined);

    // Close the file
    fclose(fp);

    return 0;
}
