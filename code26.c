#include <stdio.h>   // For printf
#include <stdlib.h>  // For rand and srand
#include <time.h>    // For time

// Function prototype
int getMonthlySales(int month);

int main() {
    // Initialize random seed
    srand(time(NULL));

    int sum = 0; // Variable to hold the sum of sales

    // Iterate three times to simulate the first quarter sales
    for (int i = 0; i < 3; i++) {
        sum += getMonthlySales(i); // Add the return value of getMonthlySales to sum
    }

    // Print the total sales for the first quarter
    printf("Total sales for the first quarter: %d\n", sum);

    return 0;
}

// Function to simulate monthly sales generation
int getMonthlySales(int month) {
    // You could enhance this function to use 'month' for specific logic
    return rand() % 100001; // Return a random sales number between 0 and 100000
}
