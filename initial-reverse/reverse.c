#include <stdio.h>    // Standard I/O operations (printf, fopen, etc.)
#include <stdlib.h>   // Standard library for memory allocation (malloc) and exit()
#include <string.h>   // String manipulation functions (strcmp, strdup)
#include <sys/stat.h> // File status retrieval for the same-file check

// Helper to reliably check if two paths point to the exact same physical file
int is_same_file(const char *path1, const char *path2) {
    struct stat stat1, stat2; // Structs to hold file metadata
    if (stat(path1, &stat1) < 0 || stat(path2, &stat2) < 0) return 0; // Return 0 if we can't read file stats
    return (stat1.st_dev == stat2.st_dev) && (stat1.st_ino == stat2.st_ino); // Compare device and inode numbers
}

// Node for our LIFO linked list (stack)
typedef struct Node {
    char *line;         // Pointer to the stored string
    struct Node *next;  // Pointer to the next node in the stack
} Node;

int main(int argc, char *argv[]) {

    // Enforce the correct maximum number of command-line arguments
    if (argc > 3) {
        fprintf(stderr, "usage: reverse <input> <output>\n"); // Print usage error to standard error
        exit(1); // Exit with a non-zero status code indicating failure
    }

    // Prevent overwriting the input file with the output file
    if (argc == 3) {
        if (strcmp(argv[1], argv[2]) == 0 || is_same_file(argv[1], argv[2])) {
            fprintf(stderr, "Input and output file must differ\n"); // Print error if files are identical
            exit(1); // Exit the program
        }
    }

    // Default to standard streams for input and output
    FILE *in = stdin;
    FILE *out = stdout;

    // Handle input file opening if provided
    if (argc >= 2) {
        in = fopen(argv[1], "r"); // Open the first argument as read-only
        if (in == NULL) {
            fprintf(stderr, "error: cannot open file '%s'\n", argv[1]); // Report error if file opening fails
            exit(1); // Exit the program
        }
    }

    // Handle output file opening if provided
    if (argc == 3) {
        out = fopen(argv[2], "w"); // Open the second argument as write-only
        if (out == NULL) {
            fprintf(stderr, "error: cannot open file '%s'\n", argv[2]); // Report error if file opening fails
            exit(1); // Exit the program
        }
    }

    Node *head = NULL;   // Initialize the top of the stack as empty
    char *buffer = NULL; // Initialize buffer pointer for getline
    size_t len = 0;      // Initialize buffer size for getline
    ssize_t nread;       // Variable to store the number of characters read

    // Build the stack by reading line by line
    while ((nread = getline(&buffer, &len, in)) != -1) {
        Node *new_node = malloc(sizeof(Node)); // Allocate memory for a new list node
        if (new_node == NULL) {
            fprintf(stderr, "malloc failed\n"); // Report allocation failure
            exit(1); // Exit the program
        }

        new_node->line = strdup(buffer); // Duplicate the read string into the node's memory
        if (new_node->line == NULL) {
            fprintf(stderr, "malloc failed\n"); // Report string duplication failure
            exit(1); // Exit the program
        }

        new_node->next = head; // Point the new node to the current top of the stack
        head = new_node;       // Update the stack top to be the newly created node
    }
    free(buffer); // Free the buffer dynamically allocated by getline

    // Print reversed lines and free memory iteratively
    Node *current = head; // Start iterating from the top of the stack
    while (current != NULL) {
        fprintf(out, "%s", current->line); // Write the stored line to the output stream

        Node *temp = current;     // Temporarily hold the current node so we can safely free it
        current = current->next;  // Move the pointer to the next node in the stack

        free(temp->line); // Free the duplicated string memory
        free(temp);       // Free the node memory itself
    }

    // Graceful closure of file streams
    if (in != stdin) fclose(in);   // Close the input file if it is not standard input
    if (out != stdout) fclose(out); // Close the output file if it is not standard output

    return 0; // Return success status code to the operating system
}
