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

    uint data_start = sb->bmapstart + (sb->size / BPB) + 1;

    // Check 2: Valid block addresses
    for (int i = 0; i < sb->ninodes; i++) {
        if (dip[i].type == 0) continue;

        // Direct blocks
        for (int j = 0; j < NDIRECT; j++) {
            uint addr = dip[i].addrs[j];
            if (addr == 0) continue;
            if (addr < data_start || addr >= sb->size) {
                fprintf(stderr, "ERROR: bad direct address in inode.\n");
                exit(1);
            }
        }

        // Indirect block
        uint indirect_addr = dip[i].addrs[NDIRECT];
        if (indirect_addr != 0) {
            if (indirect_addr < data_start || indirect_addr >= sb->size) {
                fprintf(stderr, "ERROR: bad indirect address in inode.\n");
                exit(1);
            }
            uint *indirect_blocks = (uint *)(img_ptr + (indirect_addr * BSIZE));
            for (int j = 0; j < NINDIRECT; j++) {
                uint addr = indirect_blocks[j];
                if (addr == 0) continue;
                if (addr < data_start || addr >= sb->size) {
                    fprintf(stderr, "ERROR: bad indirect address in inode.\n");
                    exit(1);
                }
            }
        }
    }

    // Check 3: Root directory exists, its inode number is 1, and the parent of the root directory is itself.
    if (dip[ROOTINO].type != T_DIR) {
        fprintf(stderr, "ERROR: root directory does not exist.\n");
        exit(1);
    }

    int found_root_parent = 0;
    for (int j = 0; j < NDIRECT; j++) {
        uint addr = dip[ROOTINO].addrs[j];
        if (addr == 0) continue;
        struct dirent *de = (struct dirent *)(img_ptr + (addr * BSIZE));
        for (int k = 0; k < BSIZE / sizeof(struct dirent); k++) {
            if (strcmp(de[k].name, "..") == 0) {
                if (de[k].inum == ROOTINO) {
                    found_root_parent = 1;
                }
            }
        }
    }
    // Also check indirect if not found in direct? Normally root is small.
    // For completeness, we should.
    if (!found_root_parent && dip[ROOTINO].addrs[NDIRECT] != 0) {
        uint *indirect_blocks = (uint *)(img_ptr + (dip[ROOTINO].addrs[NDIRECT] * BSIZE));
        for (int j = 0; j < NINDIRECT; j++) {
            if (indirect_blocks[j] == 0) continue;
            struct dirent *de = (struct dirent *)(img_ptr + (indirect_blocks[j] * BSIZE));
            for (int k = 0; k < BSIZE / sizeof(struct dirent); k++) {
                if (strcmp(de[k].name, "..") == 0) {
                    if (de[k].inum == ROOTINO) {
                        found_root_parent = 1;
                    }
                }
            }
        }
    }

    if (!found_root_parent) {
        fprintf(stderr, "ERROR: root directory does not exist.\n");
        exit(1);
    }

    // Check 4: Each directory contains . and .. entries, and the . entry points to the directory itself.
    for (int i = 0; i < sb->ninodes; i++) {
        if (dip[i].type != T_DIR) continue;

        int found_dot = 0;
        int found_dot_dot = 0;

        for (int j = 0; j < NDIRECT; j++) {
            uint addr = dip[i].addrs[j];
            if (addr == 0) continue;
            struct dirent *de = (struct dirent *)(img_ptr + (addr * BSIZE));
            for (int k = 0; k < BSIZE / sizeof(struct dirent); k++) {
                if (strcmp(de[k].name, ".") == 0) {
                    if (de[k].inum == i) found_dot = 1;
                }
                if (strcmp(de[k].name, "..") == 0) {
                    found_dot_dot = 1;
                }
            }
        }
        // Check indirect for completeness (though . and .. are always in the first block)
        if ((!found_dot || !found_dot_dot) && dip[i].addrs[NDIRECT] != 0) {
            uint *indirect_blocks = (uint *)(img_ptr + (dip[i].addrs[NDIRECT] * BSIZE));
            for (int j = 0; j < NINDIRECT; j++) {
                if (indirect_blocks[j] == 0) continue;
                struct dirent *de = (struct dirent *)(img_ptr + (indirect_blocks[j] * BSIZE));
                for (int k = 0; k < BSIZE / sizeof(struct dirent); k++) {
                    if (strcmp(de[k].name, ".") == 0) {
                        if (de[k].inum == i) found_dot = 1;
                    }
                    if (strcmp(de[k].name, "..") == 0) {
                        found_dot_dot = 1;
                    }
                }
            }
        }

        if (!found_dot || !found_dot_dot) {
            fprintf(stderr, "ERROR: directory not properly formatted.\n");
            exit(1);
        }
    }

    int *usage_count = calloc(sb->size, sizeof(int));
    if (!usage_count) {
        perror("calloc");
        exit(1);
    }

    // Identify which blocks are used by inodes and catch duplicates (Checks 7 & 8)
    for (int i = 0; i < sb->ninodes; i++) {
        if (dip[i].type == 0) continue;

        for (int j = 0; j < NDIRECT; j++) {
            uint addr = dip[i].addrs[j];
            if (addr == 0) continue;
            if (usage_count[addr] > 0) {
                fprintf(stderr, "ERROR: direct address used more than once.\n");
                exit(1);
            }
            usage_count[addr]++;
        }

        uint indirect_addr = dip[i].addrs[NDIRECT];
        if (indirect_addr != 0) {
            if (usage_count[indirect_addr] > 0) {
                // If the indirect block itself is used multiple times.
                // It's technically an address in an inode. 
                // Let's treat it as a direct address for the sake of the error message 
                // if it's in the inode's addrs array.
                fprintf(stderr, "ERROR: direct address used more than once.\n");
                exit(1);
            }
            usage_count[indirect_addr]++;

            uint *indirect_blocks = (uint *)(img_ptr + (indirect_addr * BSIZE));
            for (int j = 0; j < NINDIRECT; j++) {
                uint addr = indirect_blocks[j];
                if (addr == 0) continue;
                if (usage_count[addr] > 0) {
                    fprintf(stderr, "ERROR: indirect address used more than once.\n");
                    exit(1);
                }
                usage_count[addr]++;
            }
        }
    }

    // Check 5 & 6: Bitmap consistency
    void *bmap = img_ptr + (sb->bmapstart * BSIZE);
    for (uint b = 0; b < sb->size; b++) {
        int bit = (*((char*)bmap + (b/8)) >> (b%8)) & 0x1;
        
        if (usage_count[b] > 0 && bit == 0) {
            fprintf(stderr, "ERROR: address used by inode but marked free in bitmap.\n");
            exit(1);
        }
        
        if (bit == 1 && usage_count[b] == 0 && b >= data_start) {
            fprintf(stderr, "ERROR: bitmap marks block in use but it is not in use.\n");
            exit(1);
        }
    }

    return 0;
}
