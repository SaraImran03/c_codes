#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h> // For handling variable arguments

/**
 * Executes a command using the system call.
 * @param format A format string similar to that used in printf, containing placeholders for arguments.
 * @param ... Variable arguments that will replace the placeholders in the format string.
 */
void execute_command(const char *format, ...) {
    char command[256]; // Buffer to hold the built command
    va_list args;

    // Start processing variable arguments
    va_start(args, format);
    // Create the command string
    vsnprintf(command, sizeof(command), format, args);
    // Clean up the list
    va_end(args);

    printf("Executing command: %s\n", command);

    // Execute the command
    int result = system(command);
    if (result != 0) {
        printf("Command execution failed with status: %d\n", result);
    }
}

int main() {
    // Example usage of the execute_command function
    execute_command("ls %s %s", "-l", "-a"); // Executes 'ls -l -a'
    execute_command("ls %s", "-l");          // Executes 'ls -l'

    return 0;
}
