#include <stdio.h>   // For printf
#include <stdlib.h>  // For rand and srand
#include <time.h>    // For time

int main() {
    // Initialize random seed
    srand(time(NULL));

    // Generate three random integers greater than 1000
    int a = rand() % 9000 + 1001; // Range: 1001 to 9000
    int b = rand() % 9000 + 1001; // Range: 1001 to 9000
    int c = rand() % 9000 + 1001; // Range: 1001 to 9000

    // Print the three integers
    printf("a : %d b : %d c : %d\n", a, b, c);

    // Add the three integers
    int sum = a + b + c;
    printf("sum: %d\n", sum);

    // Add the three integers using a long long
    long long sum2 = (long long) a + b + c;
    printf("sum2: %lld\n", sum2);

    return 0;
}
