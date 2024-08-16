#include <stdio.h>

// Global array for demonstration
int id_sequence[] = {10, 20, 30, 40, 50};

// Function that prompts for an index and returns a value from the array or 0 if out of bounds
int getValueFromUser() {
    int index;
    printf("Enter an index: ");
    scanf("%d", &index);  // Read an index value from the user

    int size = sizeof(id_sequence) / sizeof(id_sequence[0]);  // Calculate the size of the array
    if (index < 0 || index >= size) {
        return 0;  // Index is out of bounds, return 0
    }
    return id_sequence[index];  // Return the value at the given index
}

int main() {
    int value = getValueFromUser();  // Call the function to get a value based on user input
    if (value == 0) {
        printf("Index out of bounds or value at index is 0.\n");
    } else {
        printf("Value at index: %d\n", value);
    }
    return 0;  // Return 0 indicating the program ran successfully
}
