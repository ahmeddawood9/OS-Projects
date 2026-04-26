#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        return 0;
    }

    for (int i = 1; i < argc; i++) {
        char *arg = strdup(argv[i]);
        char *cmd = strsep(&arg, ",");
        
        if (cmd == NULL) {
            printf("bad command\n");
            continue;
        }

        if (strcmp(cmd, "p") == 0) {
            // put
        } else if (strcmp(cmd, "g") == 0) {
            // get
        } else if (strcmp(cmd, "d") == 0) {
            // delete
        } else if (strcmp(cmd, "c") == 0) {
            // clear
        } else if (strcmp(cmd, "a") == 0) {
            // all
        } else {
            printf("bad command\n");
        }
        free(arg);
    }

    return 0;
}
