#include <stdio.h>
#include <stdlib.h>

int main() {
    // Define an array of strings for vehicles
    const char *vehicles[] = {"Car", "Motorcycle", "Bicycle", "Truck", "Bus"};
    int numVehicles = sizeof(vehicles) / sizeof(vehicles[0]); // Determine the number of vehicles in the array

    // Prompt the user for the index of a vehicle
    printf("Enter the index of a vehicle (0 to %d): ", numVehicles - 1);
    char input[10];
    fgets(input, 10, stdin); // Read input from the user
    int index = atoi(input); // Convert the input to an integer

    // Validate the index and display the selected vehicle
    if (index < 0 || index >= numVehicles) {
        printf("Invalid input. Please enter a valid index.\n");
    } else {
        printf("Vehicle at index %d: %s\n", index, vehicles[index]);
    }

    // Loop through the array and print out each vehicle
    printf("Listing all vehicles:\n");
    for (int i = 0; i < numVehicles; i++) {
        printf("%d: %s\n", i, vehicles[i]);
    }

    return 0;
}
