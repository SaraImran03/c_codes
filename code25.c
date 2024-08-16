#include <stdio.h>   // For printf
#include <stdlib.h>  // For rand and srand
#include <time.h>    // For time

// Function prototype
int getMonthlySales();

int main() {
    // Initialize random seed
    srand(time(NULL));

    int totalSales[4] = {0, 0, 0, 0}; // Array to store total sales for each quarter
    int monthSales, quarter;

    // Iterate over each month
    for (int month = 0; month < 12; month++) {
        monthSales = getMonthlySales(); // Get sales for the month
        quarter = month / 3; // Determine the current quarter
        totalSales[quarter] += monthSales; // Add month's sales to the correct quarter
    }

    // Print the total sales for each quarter
    printf("Sales for Q1: %d\n", totalSales[0]);
    printf("Sales for Q2: %d\n", totalSales[1]);
    printf("Sales for Q3: %d\n", totalSales[2]);
    printf("Sales for Q4: %d\n", totalSales[3]);

    return 0;
}

// Function to simulate monthly sales generation
int getMonthlySales() {
    return rand() % 100001; // Return a random sales number between 0 and 100000
}
