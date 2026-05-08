#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include "include/fs.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: xcheck <file_system_image>\n");
        exit(1);
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "image not found.\n");
        exit(1);
    }

    struct stat sbuf;
    fstat(fd, &sbuf);

    void *img_ptr = mmap(NULL, sbuf.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (img_ptr == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    // Basic setup complete
    struct superblock *sb = (struct superblock *)(img_ptr + BSIZE);

    struct dinode *dip = (struct dinode *)(img_ptr + (sb->inodestart * BSIZE));

    // Check 1: Each inode is either unallocated or one of the valid types
    for (int i = 0; i < sb->ninodes; i++) {
        if (dip[i].type != 0 && dip[i].type != T_FILE && dip[i].type != T_DIR && dip[i].type != T_DEV) {
            fprintf(stderr, "ERROR: bad inode.\n");
            exit(1);
        }
    }

    return 0;
}
