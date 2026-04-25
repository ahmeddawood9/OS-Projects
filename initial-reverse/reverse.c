#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc > 3) {
        fprintf(stderr, "usage: reverse <input> <output>\n");
        exit(1);
    }

    // Default to standard streams
    FILE *in = stdin;
    FILE *out = stdout;

    // Handle input file
    if (argc >= 2) {
        in = fopen(argv[1], "r");
        if (in == NULL) {
            fprintf(stderr, "error: cannot open file '%s'\n", argv[1]);
            exit(1);
        }
    }

    // Handle output file
    if (argc == 3) {
        out = fopen(argv[2], "w");
        if (out == NULL) {
            fprintf(stderr, "error: cannot open file '%s'\n", argv[2]);
            exit(1);
        }
    }

    // Read data using getline
    char *buffer = NULL;
    size_t len = 0;
    ssize_t nread;

    while ((nread = getline(&buffer, &len, in)) != -1) {
        // Temporarily just print it straight to out
        fprintf(out, "%s", buffer);
    }

    free(buffer); // Clean up the buffer allocated by getline

    // Graceful closure
    if (in != stdin) fclose(in);
    if (out != stdout) fclose(out);

    return 0;
}
