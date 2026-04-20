#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    // If no files are passed, print the required error message and exit with status 1
    if (argc < 2) {
        printf("wunzip: file1 [file2 ...]\n");
        exit(1);
    }

    // Iterate through all files provided via command line arguments
    for (int i = 1; i < argc; i++) {
        FILE *fp = fopen(argv[i], "r");

        // If a file cannot be opened, print the error and exit with status 1
        if (fp == NULL) {
            printf("wunzip: cannot open file\n");
            exit(1);
        }

        int count;
        char c;

        // fread reads the 4-byte binary integer first
        while (fread(&count, sizeof(int), 1, fp) == 1) {
            // Then it reads the 1-byte character
            if (fread(&c, sizeof(char), 1, fp) == 1) {
                // Print the character 'count' times to stdout
                for (int j = 0; j < count; j++) {
                    printf("%c", c);
                }
            }
        }

        fclose(fp);
    }

    return 0;
}
