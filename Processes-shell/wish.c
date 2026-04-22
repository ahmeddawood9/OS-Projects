#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void print_error() {
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message));
}

int main(int argc, char *argv[]) {
    if (argc > 2) { print_error(); exit(1); }
    char *line = NULL;
    size_t len = 0;
    while (1) {
        printf("wish> ");
        if (getline(&line, &len, stdin) == -1) break;
        if (strcmp(line, "exit\n") == 0) exit(0);
    }
    free(line);
    return 0;
}
