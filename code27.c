#include <stdio.h>   // For printf
#include <stdlib.h>  // For rand and srand
#include <time.h>    // For time

// Function prototype
int getMonthlySales();

int main() {
    // Initialize random seed
    srand(time(NULL));

    // Declare the totalSales variable
    int totalSales = 0;

    // Add the sales for each of the three months to totalSales
    totalSales += getMonthlySales(); // Sales for month 1
    totalSales += getMonthlySales(); // Sales for month 2
    totalSales += getMonthlySales(); // Sales for month 3

    // Print the total sales to the screen
    printf("Total sales for the quarter: %d\n", totalSales);

    return 0;
}

// Function to simulate monthly sales generation
int getMonthlySales() {
    return rand() % 100001; // Return a random sales number between 0 and 100000
}
