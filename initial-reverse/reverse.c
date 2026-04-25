#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    // Validate argument count
    if (argc > 3) {
        fprintf(stderr, "usage: reverse <input> <output>\n");
        exit(1);
    }

    return 0;
}
