#include <stdio.h>   // For printf
#include <stdlib.h>  // For rand and srand
#include <time.h>    // For time

int main() {
    // Initialize random seed
    srand(time(NULL));

    // Generate two random integers between 10 and 1000
    int num1 = rand() % 991 + 10;  // Range: 10 to 1000
    int num2 = rand() % 991 + 10;  // Range: 10 to 1000

    // Calculate the difference between the two integers
    int difference = num1 - num2;

    // Print the results
    printf("First number: %d\nSecond number: %d\nDifference: %d\n", num1, num2, difference);

    return 0;
}
