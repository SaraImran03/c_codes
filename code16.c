#include <stdio.h>

// Define an array with some example values
int id_sequence[] = {3, 5, 8, 13, 21, 34, 55, 89, 144, 233};

// Function to get value from the array based on input index
int getValueAtIndex(int index) {
    int arraySize = sizeof(id_sequence) / sizeof(id_sequence[0]); // Calculate the size of the array
    if (index < 0 || index >= arraySize) {
        return -1;  // Return -1 if index is out of bounds
    }
    return id_sequence[index];  // Return the value at the index if within bounds
}

int main() {
    int index, value;

    printf("Enter an index: ");
    scanf("%d", &index);  // Read an index value from the user

    value = getValueAtIndex(index);  // Fetch the value at the provided index
    printf("The value at index %d is %d.\n", index, value);

    return 0;  // Exit the program
}
