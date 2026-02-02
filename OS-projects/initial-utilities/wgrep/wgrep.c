#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;

    // Case 1: No search term
    if (argc < 2) {
        printf("wgrep: searchterm [file ...]\n");
        return 1;
    }

    char *search = argv[1];

    // Case 2: Search term only → read from stdin
    if (argc == 2) {
        while ((nread = getline(&line, &len, stdin)) != -1) {
            if (strstr(line, search) != NULL) {
                printf("%s", line);
            }
        }
        free(line);
        return 0;
    }

    // Case 3: Search term + files
    for (int i = 2; i < argc; i++) {
        FILE *fp = fopen(argv[i], "r");
        if (fp == NULL) {
            printf("wgrep: cannot open file\n");
            free(line);
            return 1;
        }

        while ((nread = getline(&line, &len, fp)) != -1) {
            if (strstr(line, search) != NULL) {
                printf("%s", line);
            }
        }

        fclose(fp);
    }

    free(line);
    return 0;
}
1
