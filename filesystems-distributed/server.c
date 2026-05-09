#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "udp.h"
#include "mfs.h"

#include "mfs_proto.h"

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

checkpoint_region_t cr;
int fs_fd;
int imap[MAX_INODES];

void get_inode(int inum, inode_t *inode) {
    if (inum < 0 || inum >= MAX_INODES || imap[inum] == -1) return;
    lseek(fs_fd, imap[inum], SEEK_SET);
    read(fs_fd, inode, sizeof(inode_t));
}

void write_to_log(void *data, int size, int *pos) {
    *pos = cr.end_of_log;
    lseek(fs_fd, *pos, SEEK_SET);
    write(fs_fd, data, size);
    cr.end_of_log += size;
}

void update_imap(int inum, int inode_pos) {
    imap[inum] = inode_pos;
    int piece_idx = inum / 16;
    imap_piece_t piece;
    memcpy(piece.inode_ptrs, &imap[piece_idx * 16], sizeof(imap_piece_t));
    
    int piece_pos;
    write_to_log(&piece, sizeof(imap_piece_t), &piece_pos);
    cr.imap_ptrs[piece_idx] = piece_pos;
}

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
                case MFS_LOOKUP_MSG: {
                    inode_t pinode;
                    get_inode(msg.pinum, &pinode);
                    if (pinode.type == MFS_DIRECTORY) {
                        for (int i = 0; i < 14; i++) {
                            if (pinode.pointers[i] != -1) {
                                MFS_DirEnt_t entries[MFS_BLOCK_SIZE / sizeof(MFS_DirEnt_t)];
                                lseek(fs_fd, pinode.pointers[i], SEEK_SET);
                                read(fs_fd, entries, MFS_BLOCK_SIZE);
                                for (int j = 0; j < MFS_BLOCK_SIZE / sizeof(MFS_DirEnt_t); j++) {
                                    if (entries[j].inum != -1 && strcmp(entries[j].name, msg.name) == 0) {
                                        reply.rc = entries[j].inum;
                                        break;
                                    }
                                }
                            }
                            if (reply.rc != -1) break;
                        }
                    }
                    break;
                }
                case MFS_STAT_MSG: {
                    inode_t inode;
                    get_inode(msg.inum, &inode);
                    if (msg.inum >= 0 && msg.inum < MAX_INODES && imap[msg.inum] != -1) {
                        reply.stat.type = inode.type;
                        reply.stat.size = inode.size;
                        reply.rc = 0;
                    }
                    break;
                }
                case MFS_WRITE_MSG: {
                    inode_t inode;
                    get_inode(msg.inum, &inode);
                    if (msg.inum >= 0 && msg.inum < MAX_INODES && imap[msg.inum] != -1 && inode.type == MFS_REGULAR_FILE && msg.block >= 0 && msg.block < 14) {
                        int data_pos;
                        write_to_log(msg.buffer, MFS_BLOCK_SIZE, &data_pos);
                        inode.pointers[msg.block] = data_pos;
                        if ((msg.block + 1) * MFS_BLOCK_SIZE > inode.size) {
                            inode.size = (msg.block + 1) * MFS_BLOCK_SIZE;
                        }
                        int inode_pos;
                        write_to_log(&inode, sizeof(inode_t), &inode_pos);
                        update_imap(msg.inum, inode_pos);
                        lseek(fs_fd, 0, SEEK_SET);
                        write(fs_fd, &cr, sizeof(checkpoint_region_t));
                        fsync(fs_fd);
                        reply.rc = 0;
                    }
                    break;
                }
                case MFS_READ_MSG: {
                    inode_t inode;
                    get_inode(msg.inum, &inode);
                    if (msg.inum >= 0 && msg.inum < MAX_INODES && imap[msg.inum] != -1 && msg.block >= 0 && msg.block < 14) {
                        if (inode.pointers[msg.block] != -1) {
                            lseek(fs_fd, inode.pointers[msg.block], SEEK_SET);
                            read(fs_fd, reply.buffer, MFS_BLOCK_SIZE);
                        } else {
                            memset(reply.buffer, 0, MFS_BLOCK_SIZE);
                        }
                        reply.rc = 0;
                    }
                    break;
                }
                case MFS_CREAT_MSG: {
                    inode_t pinode;
                    get_inode(msg.pinum, &pinode);
                    if (msg.pinum >= 0 && msg.pinum < MAX_INODES && imap[msg.pinum] != -1 && pinode.type == MFS_DIRECTORY) {
                        // Check if exists
                        int exists = -1;
                        for (int i = 0; i < 14; i++) {
                            if (pinode.pointers[i] != -1) {
                                MFS_DirEnt_t entries[MFS_BLOCK_SIZE / sizeof(MFS_DirEnt_t)];
                                lseek(fs_fd, pinode.pointers[i], SEEK_SET);
                                read(fs_fd, entries, MFS_BLOCK_SIZE);
                                for (int j = 0; j < MFS_BLOCK_SIZE / sizeof(MFS_DirEnt_t); j++) {
                                    if (entries[j].inum != -1 && strcmp(entries[j].name, msg.name) == 0) {
                                        exists = entries[j].inum;
                                        break;
                                    }
                                }
                            }
                            if (exists != -1) break;
                        }

                        if (exists != -1) {
                            reply.rc = 0; // Success if already exists
                        } else {
                            // Find free inode
                            int new_inum = -1;
                            for (int i = 0; i < MAX_INODES; i++) {
                                if (imap[i] == -1) {
                                    new_inum = i;
                                    break;
                                }
                            }

                            if (new_inum != -1) {
                                // Find free directory entry
                                int entry_found = 0;
                                for (int i = 0; i < 14; i++) {
                                    MFS_DirEnt_t entries[MFS_BLOCK_SIZE / sizeof(MFS_DirEnt_t)];
                                    if (pinode.pointers[i] != -1) {
                                        lseek(fs_fd, pinode.pointers[i], SEEK_SET);
                                        read(fs_fd, entries, MFS_BLOCK_SIZE);
                                    } else {
                                        for (int j = 0; j < MFS_BLOCK_SIZE / sizeof(MFS_DirEnt_t); j++) entries[j].inum = -1;
                                    }

                                    for (int j = 0; j < MFS_BLOCK_SIZE / sizeof(MFS_DirEnt_t); j++) {
                                        if (entries[j].inum == -1) {
                                            strcpy(entries[j].name, msg.name);
                                            entries[j].inum = new_inum;
                                            
                                            int data_pos;
                                            write_to_log(entries, MFS_BLOCK_SIZE, &data_pos);
                                            pinode.pointers[i] = data_pos;
                                            if ((i+1) * MFS_BLOCK_SIZE > pinode.size) pinode.size = (i+1) * MFS_BLOCK_SIZE;
                                            
                                            int pinode_pos;
                                            write_to_log(&pinode, sizeof(inode_t), &pinode_pos);
                                            update_imap(msg.pinum, pinode_pos);
                                            
                                            // New inode
                                            inode_t new_inode;
                                            new_inode.type = msg.mfs_type;
                                            for (int k = 0; k < 14; k++) new_inode.pointers[k] = -1;
                                            if (msg.mfs_type == MFS_DIRECTORY) {
                                                new_inode.size = 2 * sizeof(MFS_DirEnt_t);
                                                MFS_DirEnt_t new_entries[MFS_BLOCK_SIZE / sizeof(MFS_DirEnt_t)];
                                                for (int k = 0; k < MFS_BLOCK_SIZE / sizeof(MFS_DirEnt_t); k++) new_entries[k].inum = -1;
                                                strcpy(new_entries[0].name, ".");
                                                new_entries[0].inum = new_inum;
                                                strcpy(new_entries[1].name, "..");
                                                new_entries[1].inum = msg.pinum;
                                                
                                                int new_data_pos;
                                                write_to_log(new_entries, MFS_BLOCK_SIZE, &new_data_pos);
                                                new_inode.pointers[0] = new_data_pos;
                                            } else {
                                                new_inode.size = 0;
                                            }
                                            
                                            int new_inode_pos;
                                            write_to_log(&new_inode, sizeof(inode_t), &new_inode_pos);
                                            update_imap(new_inum, new_inode_pos);
                                            
                                            lseek(fs_fd, 0, SEEK_SET);
                                            write(fs_fd, &cr, sizeof(checkpoint_region_t));
                                            fsync(fs_fd);
                                            
                                            reply.rc = 0;
                                            entry_found = 1;
                                            break;
                                        }
                                    }
                                    if (entry_found) break;
                                }
                            }
                        }
                    }
                    break;
                }
                case MFS_UNLINK_MSG: {
                    inode_t pinode;
                    get_inode(msg.pinum, &pinode);
                    if (msg.pinum >= 0 && msg.pinum < MAX_INODES && imap[msg.pinum] != -1 && pinode.type == MFS_DIRECTORY) {
                        for (int i = 0; i < 14; i++) {
                            if (pinode.pointers[i] != -1) {
                                MFS_DirEnt_t entries[MFS_BLOCK_SIZE / sizeof(MFS_DirEnt_t)];
                                lseek(fs_fd, pinode.pointers[i], SEEK_SET);
                                read(fs_fd, entries, MFS_BLOCK_SIZE);
                                for (int j = 0; j < MFS_BLOCK_SIZE / sizeof(MFS_DirEnt_t); j++) {
                                    if (entries[j].inum != -1 && strcmp(entries[j].name, msg.name) == 0) {
                                        int inum_to_del = entries[j].inum;
                                        inode_t inode_to_del;
                                        get_inode(inum_to_del, &inode_to_del);
                                        
                                        if (inode_to_del.type == MFS_DIRECTORY) {
                                            // Check if empty
                                            int count = 0;
                                            for (int k = 0; k < 14; k++) {
                                                if (inode_to_del.pointers[k] != -1) {
                                                    MFS_DirEnt_t sub_entries[MFS_BLOCK_SIZE / sizeof(MFS_DirEnt_t)];
                                                    lseek(fs_fd, inode_to_del.pointers[k], SEEK_SET);
                                                    read(fs_fd, sub_entries, MFS_BLOCK_SIZE);
                                                    for (int l = 0; l < MFS_BLOCK_SIZE / sizeof(MFS_DirEnt_t); l++) {
                                                        if (sub_entries[l].inum != -1 && strcmp(sub_entries[l].name, ".") != 0 && strcmp(sub_entries[l].name, "..") != 0) {
                                                            count++;
                                                        }
                                                    }
                                                }
                                            }
                                            if (count > 0) {
                                                reply.rc = -1; // Directory not empty
                                                break;
                                            }
                                        }
                                        
                                        // Unlink
                                        entries[j].inum = -1;
                                        int data_pos;
                                        write_to_log(entries, MFS_BLOCK_SIZE, &data_pos);
                                        pinode.pointers[i] = data_pos;
                                        
                                        int pinode_pos;
                                        write_to_log(&pinode, sizeof(inode_t), &pinode_pos);
                                        update_imap(msg.pinum, pinode_pos);
                                        
                                        imap[inum_to_del] = -1; // Mark as free
                                        int piece_idx = inum_to_del / 16;
                                        imap_piece_t piece;
                                        memcpy(piece.inode_ptrs, &imap[piece_idx * 16], sizeof(imap_piece_t));
                                        int piece_pos;
                                        write_to_log(&piece, sizeof(imap_piece_t), &piece_pos);
                                        cr.imap_ptrs[piece_idx] = piece_pos;

                                        lseek(fs_fd, 0, SEEK_SET);
                                        write(fs_fd, &cr, sizeof(checkpoint_region_t));
                                        fsync(fs_fd);
                                        
                                        reply.rc = 0;
                                        break;
                                    }
                                }
                            }
                            if (reply.rc != -1) break;
                        }
                        if (reply.rc == -1) reply.rc = 0; // Not found is not a failure
                    }
                    break;
                }
                case MFS_SHUTDOWN_MSG: {
                    fsync(fs_fd);
                    reply.rc = 0;
                    UDP_Write(sd, &addr, (char *)&reply, sizeof(mfs_reply_t));
                    close(fs_fd);
                    exit(0);
                }                default:
                    break;
            }
            UDP_Write(sd, &addr, (char *)&reply, sizeof(mfs_reply_t));
        }
    }

    return 0;
}
