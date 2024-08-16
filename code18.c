#include <stdio.h>

// Define an array with predefined values
int id_sequence[] = {10, 15, 20, 25, 30, 35, 40, 45, 50, 55};

// Function to get the value from the array based on the input index
int getValueAtIndex(int index) {
    int size = sizeof(id_sequence) / sizeof(id_sequence[0]);  // Determine the size of the array
    if (index < 0 || index >= size) {
        return -1;  // If the index is out of bounds, return -1
    }
    return id_sequence[index];  // Return the value at the specified index
}

int main() {
    int index;

    printf("Please enter an index: ");
    scanf("%d", &index);  // Read the index value from the user

    int result = getValueAtIndex(index);  // Retrieve the value or -1 if out of bounds
    printf("Result: %d\n", result);  // Print the result

    return 0;  // Successful program termination
}
