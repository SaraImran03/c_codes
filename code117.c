#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
    float a, b, c;
    char str_a[50], str_b[50], str_c[50];

    // Seed the random number generator
    srand((unsigned int)time(NULL));

    // Generate three random floats
    a = (float)rand() / RAND_MAX * 100;  // Scale to 0-100 range
    b = (float)rand() / RAND_MAX * 100;  // Scale to 0-100 range
    c = (float)rand() / RAND_MAX * 100;  // Scale to 0-100 range

    // Convert floats to strings
    sprintf(str_a, "%.2f", a);
    sprintf(str_b, "%.2f", b);
    sprintf(str_c, "%.2f", c);

    // Print the strings
    printf("String A: %s\n", str_a);
    printf("String B: %s\n", str_b);
    printf("String C: %s\n", str_c);

    return 0;
}
