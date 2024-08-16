#include <stdio.h>

// Function to get the value from the array based on the given index
int getValueFromArray(int arr[], int size, int index) {
    if (index < 0 || index >= size) {
        return -1;  // Return -1 if the index is out of bounds
    }
    return arr[index];  // Return the value at the given index
}

int main() {
    int id_sequence[] = {10, 20, 30, 40, 50};  // Example array
    int size = sizeof(id_sequence) / sizeof(id_sequence[0]);  // Calculate the size of the array
    int index;
    
    printf("Enter an index: ");
    scanf("%d", &index);  // Read an integer index from the standard input

    // Get the value from the array using the read index and use it as the exit code
    return getValueFromArray(id_sequence, size, index);
}
