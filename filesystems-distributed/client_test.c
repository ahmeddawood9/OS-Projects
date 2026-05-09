#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "mfs.h"

int main() {
    int rc = MFS_Init("localhost", 10000);
    assert(rc == 0);

    printf("Creating file 'test.txt'...\n");
    rc = MFS_Creat(0, MFS_REGULAR_FILE, "test.txt");
    assert(rc == 0);

    printf("Looking up 'test.txt'...\n");
    int inum = MFS_Lookup(0, "test.txt");
    assert(inum > 0);
    printf("Found 'test.txt' at inode %d\n", inum);

    char buffer[MFS_BLOCK_SIZE];
    sprintf(buffer, "Hello from MFS!");
    printf("Writing to 'test.txt'...\n");
    rc = MFS_Write(inum, buffer, 0);
    assert(rc == 0);

    char read_buffer[MFS_BLOCK_SIZE];
    printf("Reading from 'test.txt'...\n");
    rc = MFS_Read(inum, read_buffer, 0);
    assert(rc == 0);
    printf("Read data: %s\n", read_buffer);
    assert(strcmp(buffer, read_buffer) == 0);

    MFS_Stat_t st;
    rc = MFS_Stat(inum, &st);
    assert(rc == 0);
    printf("File size: %d, Type: %d\n", st.size, st.type);

    printf("Creating directory 'subdir'...\n");
    rc = MFS_Creat(0, MFS_DIRECTORY, "subdir");
    assert(rc == 0);
    int sub_inum = MFS_Lookup(0, "subdir");
    assert(sub_inum > 0);

    printf("Looking up '.' in 'subdir'...\n");
    int dot_inum = MFS_Lookup(sub_inum, ".");
    assert(dot_inum == sub_inum);

    printf("Shutting down server...\n");
    rc = MFS_Shutdown();
    assert(rc == 0);

    printf("Test passed!\n");
    return 0;
}
