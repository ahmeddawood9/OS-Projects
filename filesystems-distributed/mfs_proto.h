#ifndef __MFS_PROTO_H__
#define __MFS_PROTO_H__

#include "mfs.h"

// Request structure for UDP communication
typedef enum {
    MFS_LOOKUP_MSG,
    MFS_STAT_MSG,
    MFS_WRITE_MSG,
    MFS_READ_MSG,
    MFS_CREAT_MSG,
    MFS_UNLINK_MSG,
    MFS_SHUTDOWN_MSG
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

#endif
