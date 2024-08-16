#include <stdio.h>

// Function to retrieve a value from an array based on a given index
int getValueFromArray(int arr[], int size, int index) {
    if (index >= 0 && index < size) {
        return arr[index];  // Return the value at the specified index if it is within bounds
    }
    return -1;  // Return -1 if the index is out of bounds
}

int main() {
    int id_sequence[] = {10, 20, 30, 40, 50};  // Example array of integers
    int size = sizeof(id_sequence) / sizeof(id_sequence[0]);  // Calculate the number of elements in the array
    int index;
    
    printf("Enter an index: ");
    scanf("%d", &index);  // Read an integer index from standard input

    // Call getValueFromArray with the input index and use the return value as the exit code of the program
    return getValueFromArray(id_sequence, size, index);
}
