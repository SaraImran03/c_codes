#include <stdio.h>

// Global array for demonstration
int id_sequence[] = {10, 20, 30, 40, 50};

// Function to retrieve the value from an array at a given index
int getValueAtIndex(int index) {
    int size = sizeof(id_sequence) / sizeof(id_sequence[0]);  // Calculate the size of the array
    if (index < 0 || index >= size) {
        return -1;  // Index is out of bounds, return -1
    }
    return id_sequence[index];  // Return the value at the given index
}

int main() {
    int index;
    printf("Enter an index: ");
    scanf("%d", &index);  // Read an index value from the user

    int value = getValueAtIndex(index);  // Call the function with the user-provided index
    if (value == -1) {
        printf("Index out of bounds.\n");
    } else {
        printf("Value at index %d is %d.\n", index, value);
    }
    return 0;
}
