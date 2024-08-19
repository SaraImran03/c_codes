#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h> // For handling variable arguments

/**
 * Executes a command using the system call.
 * @param format A format string for the command, similar to printf.
 * @param ... Variable arguments used to fill in the format string.
 */
void execute_ls_command(const char *format, ...) {
    char command[256];  // Buffer to hold the built command
    va_list args;       // Variable argument list

    // Initialize the variable arguments list
    va_start(args, format);
    // Format the command string
    vsnprintf(command, sizeof(command), format, args);
    // Clean up the variable arguments list
    va_end(args);

    printf("Executing command: %s\n", command);

    // Execute the command
    int result = system(command);
    if (result != 0) {
        printf("Command execution failed with status: %d\n", result);
    }
}

int main() {
    // Example usage of the execute_ls_command function
    execute_ls_command("ls %s %s", "-l", "-a");  // Executes 'ls -l -a'
    execute_ls_command("ls %s", "-lh");          // Executes 'ls -lh'

    return 0;
}
