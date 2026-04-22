#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

char **shell_path;
int path_count = 1;

void print_error() {
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message));
}

char** tokenize(char *cmd) {
    int pos = 0;
    char **tokens = malloc(64 * sizeof(char*));
    char *token;
    while ((token = strsep(&cmd, " \t\n")) != NULL) {
        if (strlen(token) == 0) continue;
        tokens[pos++] = token;
    }
    tokens[pos] = NULL;
    return tokens;
}

int main(int argc, char *argv[]) {
    shell_path = malloc(sizeof(char*));
    shell_path[0] = strdup("/bin");

    char *line = NULL;
    size_t len = 0;

    while (1) {
        if (isatty(STDIN_FILENO)) printf("wish> ");
        if (getline(&line, &len, stdin) == -1) break;

        // Remove trailing newline
        if (line[strlen(line) - 1] == '\n') line[strlen(line) - 1] = '\0';

        char **args = tokenize(line);
        if (args[0] == NULL) {
            free(args);
            continue;
        }

        // --- BUILT-INS ---
        if (strcmp(args[0], "exit") == 0) {
            if (args[1] != NULL) print_error();
            else exit(0);
        }
        else if (strcmp(args[0], "cd") == 0) {
            if (args[1] == NULL || args[2] != NULL) print_error();
            else if (chdir(args[1]) != 0) print_error();
        }
        else if (strcmp(args[0], "path") == 0) {
            // Path update logic will go here in next commit
        }
        // --- EXTERNAL COMMANDS ---
        else {
            pid_t pid = fork();
            if (pid == 0) {
                char p[512];
                snprintf(p, 512, "%s/%s", shell_path[0], args[0]);
                execv(p, args);
                print_error();
                exit(1);
            } else {
                wait(NULL);
            }
        }

        // NOW we free it, after all checks are done
        free(args);
    }
    free(line);
    return 0;
}
