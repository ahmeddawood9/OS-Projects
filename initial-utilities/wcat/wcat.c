#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    // 1. ARGUMENT CHECK
    // If argc is 1, it means only "./wcat" was typed.
    if (argc < 2) {
        exit(0);
    }

    // 2. THE OUTER LOOP
    for (int i = 1; i < argc; i++) {

        // 3. THE OPEN CALL
        // We ask the OS to give us a 'file pointer' (a handle) to the disk.

        FILE *fp = fopen(argv[i], "r") ;


        // If the OS returns NULL, the file doesn't exist or it might don't have permission.
        if (fp == NULL) {
            printf("wcat: cannot open file\n");
            exit(1); // Exit with code 1 to indicate an error happened.
        }

        // 5. THE READING LOOP
        // We need a temporary place (buffer) to store what we read from the disk.
        char buffer[512];

        // fgets() reads characters until it hits a newline or the buffer is full.
        while (fgets(buffer, 512, fp) != NULL) {
            // We print the contents of the buffer to 'Standard Output'.
            printf("%s", buffer);
        }

        // 6. THE CLOSE CALL
        // We tell the OS we are done with this file so it can free up resources.
        fclose(fp);
    }


    // If we finished all files without hitting an error, return 0.
    return 0;
}
