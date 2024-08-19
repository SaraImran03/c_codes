#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
    float num1, num2, num3;
    char string[20], string2[20], string3[20];  // Buffers for the string representations

    // Seed the random number generator for different results each run
    srand((unsigned)time(NULL));

    // Generate three random floats
    num1 = (float)rand() / RAND_MAX * 100.0;  // Generate a float between 0 and 100
    num2 = (float)rand() / RAND_MAX * 100.0;  // Generate a float between 0 and 100
    num3 = (float)rand() / RAND_MAX * 100.0;  // Generate a float between 0 and 100

    // Convert each float to a string
    sprintf(string, "%.2f", num1);
    sprintf(string2, "%.2f", num2);
    sprintf(string3, "%.2f", num3);

    // Output the results
    printf("Random float 1: %s\n", string);
    printf("Random float 2: %s\n", string2);
    printf("Random float 3: %s\n", string3);

    return 0;
}
