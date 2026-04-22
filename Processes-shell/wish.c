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

    char *line = NULL; size_t len = 0;
    while (1) {
        printf("wish> ");
        if (getline(&line, &len, stdin) == -1) break;

        char **args = tokenize(line);
        if (args[0] == NULL) { free(args); continue; }
        if (strcmp(args[0], "exit") == 0) exit(0);

        pid_t pid = fork();
        if (pid == 0) {
            char p[512]; snprintf(p, 512, "%s/%s", shell_path[0], args[0]);
            execv(p, args);
            print_error(); exit(1);
        } else { wait(NULL); }
        free(args);
    }
    return 0;
}
