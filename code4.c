#include <stdio.h>
#include <stdlib.h>

int main() {
    // Array of vehicle names
    const char *vehicles[] = {"Car", "Truck", "Bicycle", "Motorcycle", "Bus"};
    int numberOfVehicles = sizeof(vehicles) / sizeof(vehicles[0]); // Number of elements in the array

    // Variable to store user input
    char input[10];
    int index;

    // Prompt the user to enter an index
    printf("Enter the index of the vehicle (0 to %d): ", numberOfVehicles - 1);
    fgets(input, 10, stdin); // Read the string input

    // Convert the input to an integer
    index = atoi(input); // atoi safely converts a string to an integer

    // Check if the index is within the bounds of the array
    if (index < 0 || index >= numberOfVehicles) {
        printf("Invalid index! Please enter a number between 0 and %d.\n", numberOfVehicles - 1);
    } else {
        // Print the vehicle at the specified index
        printf("Vehicle at index %d: %s\n", index, vehicles[index]);
    }

    return 0;
}
