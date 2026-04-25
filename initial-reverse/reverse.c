#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Node for our LIFO linked list (stack)
typedef struct Node {
    char *line;
    struct Node *next;
} Node;

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

    Node *head = NULL;
    char *buffer = NULL;
    size_t len = 0;
    ssize_t nread;

    // Build the stack
    while ((nread = getline(&buffer, &len, in)) != -1) {
        Node *new_node = malloc(sizeof(Node));
        if (new_node == NULL) {
            fprintf(stderr, "malloc failed\n");
            exit(1);
        }

        new_node->line = strdup(buffer);
        if (new_node->line == NULL) {
            fprintf(stderr, "malloc failed\n");
            exit(1);
        }

        new_node->next = head;
        head = new_node;
    }
    free(buffer);

    // Print reversed lines and free memory iteratively
    Node *current = head;
    while (current != NULL) {
        fprintf(out, "%s", current->line);

        Node *temp = current;
        current = current->next;

        free(temp->line);
        free(temp);
    }

    // Graceful closure
    if (in != stdin) fclose(in);
    if (out != stdout) fclose(out);

    return 0;
}
