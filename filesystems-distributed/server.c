#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "udp.h"
#include "mfs.h"

#define MAX_INODES 4096
#define IMAP_PIECES (MAX_INODES / 16)

typedef struct {
    int end_of_log;
    int imap_ptrs[IMAP_PIECES];
} checkpoint_region_t;

typedef struct {
    int inode_ptrs[16];
} imap_piece_t;

typedef struct {
    int type;
    int size;
    int pointers[14];
} inode_t;

// Request structure for UDP communication
typedef enum {
    MFS_LOOKUP,
    MFS_STAT,
    MFS_WRITE,
    MFS_READ,
    MFS_CREAT,
    MFS_UNLINK,
    MFS_SHUTDOWN
} mfs_msg_type_t;

typedef struct {
    mfs_msg_type_t type;
    int pinum;
    int inum;
    int block;
    char name[28];
    char buffer[MFS_BLOCK_SIZE];
    int mfs_type; // For MFS_Creat
} mfs_msg_t;

// Response structure
typedef struct {
    int rc;
    int inum;
    MFS_Stat_t stat;
    char buffer[MFS_BLOCK_SIZE];
} mfs_reply_t;

checkpoint_region_t cr;
int fs_fd;
int imap[MAX_INODES];

void fs_init(char *image_path) {
    if (access(image_path, F_OK) == 0) {
        fs_fd = open(image_path, O_RDWR);
        read(fs_fd, &cr, sizeof(checkpoint_region_t));
        for (int i = 0; i < IMAP_PIECES; i++) {
            if (cr.imap_ptrs[i] != -1) {
                lseek(fs_fd, cr.imap_ptrs[i], SEEK_SET);
                read(fs_fd, &imap[i * 16], sizeof(imap_piece_t));
            }
        }
    } else {
        fs_fd = open(image_path, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU);
        cr.end_of_log = sizeof(checkpoint_region_t);
        for (int i = 0; i < IMAP_PIECES; i++) cr.imap_ptrs[i] = -1;
        for (int i = 0; i < MAX_INODES; i++) imap[i] = -1;

        // Create root directory
        inode_t root_inode;
        root_inode.type = MFS_DIRECTORY;
        root_inode.size = 2 * sizeof(MFS_DirEnt_t);
        for (int i = 0; i < 14; i++) root_inode.pointers[i] = -1;
        root_inode.pointers[0] = cr.end_of_log + sizeof(inode_t);

        MFS_DirEnt_t entries[MFS_BLOCK_SIZE / sizeof(MFS_DirEnt_t)];
        for (int i = 0; i < MFS_BLOCK_SIZE / sizeof(MFS_DirEnt_t); i++) entries[i].inum = -1;
        strcpy(entries[0].name, ".");
        entries[0].inum = 0;
        strcpy(entries[1].name, "..");
        entries[1].inum = 0;

        int inode_pos = cr.end_of_log;
        int data_pos = inode_pos + sizeof(inode_t);
        cr.end_of_log = data_pos + MFS_BLOCK_SIZE;

        imap[0] = inode_pos;
        imap_piece_t piece;
        memcpy(piece.inode_ptrs, &imap[0], sizeof(imap_piece_t));
        int imap_pos = cr.end_of_log;
        cr.imap_ptrs[0] = imap_pos;
        cr.end_of_log += sizeof(imap_piece_t);

        lseek(fs_fd, 0, SEEK_SET);
        write(fs_fd, &cr, sizeof(checkpoint_region_t));
        lseek(fs_fd, inode_pos, SEEK_SET);
        write(fs_fd, &root_inode, sizeof(inode_t));
        lseek(fs_fd, data_pos, SEEK_SET);
        write(fs_fd, entries, MFS_BLOCK_SIZE);
        lseek(fs_fd, imap_pos, SEEK_SET);
        write(fs_fd, &piece, sizeof(imap_piece_t));
        
        fsync(fs_fd);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: server [portnum] [file-system-image]\n");
        exit(1);
    }

    int port = atoi(argv[1]);
    char *image_path = argv[2];

    fs_init(image_path);

    int sd = UDP_Open(port);
    if (sd <= 0) {
        fprintf(stderr, "Failed to open UDP port %d\n", port);
        exit(1);
    }

    printf("Server listening on port %d...\n", port);

    struct sockaddr_in addr;
    mfs_msg_t msg;
    while (1) {
        int rc = UDP_Read(sd, &addr, (char *)&msg, sizeof(mfs_msg_t));
        if (rc > 0) {
            mfs_reply_t reply;
            reply.rc = -1;

            switch (msg.type) {
                case MFS_SHUTDOWN:
                    fsync(fs_fd);
                    reply.rc = 0;
                    UDP_Write(sd, &addr, (char *)&reply, sizeof(mfs_reply_t));
                    close(fs_fd);
                    exit(0);
                default:
                    // TODO: Implement other handlers
                    break;
            }
            UDP_Write(sd, &addr, (char *)&reply, sizeof(mfs_reply_t));
        }
    }

    return 0;
}
