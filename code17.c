#include <stdio.h>

// Define an array with predefined values
int id_sequence[] = {3, 5, 8, 13, 21, 34, 55, 89, 144, 233};

// Function to retrieve the value at a specified index or return -1 if out of bounds
int getValueAtIndex(int index) {
    int arraySize = sizeof(id_sequence) / sizeof(id_sequence[0]); // Calculate the size of the array
    if (index < 0 || index >= arraySize) {
        return -1;  // Index is out of bounds
    }
    return id_sequence[index];  // Return the value at the index
}

int main() {
    int index;

    printf("Enter an index: ");
    scanf("%d", &index);  // Read an index value from the user

    int value = getValueAtIndex(index);  // Retrieve the value from the array or -1 if out of bounds
    if (value == -1) {
        printf("Index out of bounds.\n");
    } else {
        printf("Value at index %d is %d.\n", index, value);
    }

    return 0;  // Exit the program
}
