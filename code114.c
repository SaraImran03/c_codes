#include <stdio.h>
#include <stdlib.h>

/**
 * Executes the 'ls' command with additional options.
 * @param format A format string similar to that used in printf.
 * @param ... Variable arguments providing additional options to the ls command.
 */
void execute_ls_command(const char *format, ...) {
    char command[256]; // Buffer to hold the built command
    va_list args;

    // Start variadic argument processing
    va_start(args, format);
    // Create the command string
    vsprintf(command, format, args);
    // End variadic argument processing
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
    execute_ls_command("ls %s %s", "-l", "-a"); // Executes 'ls -l -a'

    return 0;
}
