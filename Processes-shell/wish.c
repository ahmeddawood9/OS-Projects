/*
 * WISH: The Wisconsin Shell.
 * Part of the Operating Systems Projects collection.
 *
 * A custom Unix-style command-line interpreter with support for built-in
 * commands, I/O redirection, and parallel execution.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

char **shell_path;
int path_count = 0;

void print_error() {
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message));
}

char** tokenize(char *cmd, const char *delim) {
    int bufsize = 64, pos = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    char *token;
    if (!tokens) exit(1);

    while ((token = strsep(&cmd, delim)) != NULL) {
        if (strlen(token) == 0) continue;

        // Clean up trailing newline if present
        if (token[strlen(token) - 1] == '\n') token[strlen(token) - 1] = '\0';
        if (strlen(token) == 0) continue;

        tokens[pos++] = token;
        if (pos >= bufsize) {
            bufsize += 64;
            tokens = realloc(tokens, bufsize * sizeof(char*));
        }
    }
    tokens[pos] = NULL;
    return tokens;
}

int execute_cmd(char *command_line) {
    char *out_file = NULL;
    char *redir_ptr = strchr(command_line, '>');

    // 1. Strict Redirection Parsing
    if (redir_ptr) {
        *redir_ptr = '\0';
        char *file_part = redir_ptr + 1;

        // Error: multiple redirection symbols (e.g., ls > f1 > f2)
        if (strchr(file_part, '>') != NULL) {
            print_error();
            return -1;
        }

        char **file_tokens = tokenize(file_part, " \t\n");
        // Error: no file or multiple files (e.g., ls > or ls > f1 f2)
        if (file_tokens[0] == NULL || file_tokens[1] != NULL) {
            print_error();
            free(file_tokens);
            return -1;
        }
        out_file = file_tokens[0];
        // file_tokens[0] is just a pointer into command_line, so we can free the array
        free(file_tokens);
    }

    char **args = tokenize(command_line, " \t\n");

    // Error: redirection with no command (e.g., > file.txt)
    if (args[0] == NULL) {
        if (redir_ptr) print_error();
        free(args);
        return -1;
    }

    // 2. Built-ins
    if (strcmp(args[0], "exit") == 0) {
        if (args[1] != NULL) print_error();
        else exit(0);
    } else if (strcmp(args[0], "cd") == 0) {
        if (args[1] == NULL || args[2] != NULL) print_error();
        else if (chdir(args[1]) != 0) print_error();
    } else if (strcmp(args[0], "path") == 0) {
        // Free old paths
        for (int i = 0; i < path_count; i++) free(shell_path[i]);
        free(shell_path);

        // Count new paths
        path_count = 0;
        while (args[path_count + 1] != NULL) path_count++;

        shell_path = malloc((path_count + 1) * sizeof(char*));
        for (int i = 0; i < path_count; i++) {
            shell_path[i] = strdup(args[i + 1]);
        }
    } else {
        // 3. External Execution
        char *executable = NULL;
        for (int i = 0; i < path_count; i++) {
            char p[512];
            snprintf(p, 512, "%s/%s", shell_path[i], args[0]);
            if (access(p, X_OK) == 0) {
                executable = strdup(p);
                break;
            }
        }

        if (!executable) {
            print_error();
        } else {
            pid_t pid = fork();
            if (pid == 0) { // Child
                if (out_file) {
                    int fd = open(out_file, O_WRONLY | O_CREAT | O_TRUNC, 0666);
                    if (fd < 0) { print_error(); exit(1); }
                    dup2(fd, STDOUT_FILENO);
                    dup2(fd, STDERR_FILENO);
                    close(fd);
                }
                execv(executable, args);
                exit(1); // Should never reach here
            }
            free(executable);
            free(args);
            return pid; // Return for parallel waiting
        }
    }
    free(args);
    return -1;
}

int main(int argc, char *argv[]) {
    FILE *input = stdin;
    int interactive = 1;

    // Initial Path: /bin
    shell_path = malloc(sizeof(char*));
    shell_path[0] = strdup("/bin");
    path_count = 1;

    if (argc > 2) { print_error(); exit(1); }
    if (argc == 2) {
        input = fopen(argv[1], "r");
        if (!input) { print_error(); exit(1); }
        interactive = 0;
    }

    char *line = NULL;
    size_t len = 0;
    while (1) {
        if (interactive) { printf("wish> "); fflush(stdout); }

        if (getline(&line, &len, input) == -1) break;

        char *temp_line = line;
        char *command;
        pid_t pids[64];
        int cmd_idx = 0;

        // Process parallel commands separated by '&'
        while ((command = strsep(&temp_line, "&")) != NULL) {
            // Clean up the command string from strsep
            if (strlen(command) == 0 || strcmp(command, "\n") == 0) continue;

            pid_t pid = execute_cmd(command);
            if (pid > 0) {
                if (cmd_idx < 64) pids[cmd_idx++] = pid;
            }
        }

        // Wait for all commands on this line to finish
        for (int i = 0; i < cmd_idx; i++) {
            waitpid(pids[i], NULL, 0);
        }
    }

    // Cleanup before exit
    for (int i = 0; i < path_count; i++) free(shell_path[i]);
    free(shell_path);
    free(line);
    if (input != stdin) fclose(input);

    return 0;
}
