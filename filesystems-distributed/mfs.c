#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include "udp.h"
#include "mfs.h"
#include "mfs_proto.h"

static int sd = -1;
static struct sockaddr_in addr;

int MFS_Init(char *hostname, int port) {
    sd = UDP_Open(0); // Open on any available port
    if (sd < 0) return -1;
    return UDP_FillSockAddr(&addr, hostname, port);
}

static int MFS_Send_Receive(mfs_msg_t *msg, mfs_reply_t *reply) {
    struct timeval tv;
    fd_set readfds;
    int retries = 0;

    while (retries < 10) { // Max retries
        UDP_Write(sd, &addr, (char *)msg, sizeof(mfs_msg_t));

        tv.tv_sec = 5;
        tv.tv_usec = 0;
        FD_ZERO(&readfds);
        FD_SET(sd, &readfds);

        int rc = select(sd + 1, &readfds, NULL, NULL, &tv);
        if (rc > 0) {
            struct sockaddr_in reply_addr;
            int n = UDP_Read(sd, &reply_addr, (char *)reply, sizeof(mfs_reply_t));
            if (n > 0) return 0;
        }
        retries++;
    }
    return -1;
}

int MFS_Lookup(int pinum, char *name) {
    if (pinum < 0 || name == NULL || strlen(name) >= 28) return -1;
    mfs_msg_t msg;
    mfs_reply_t reply;
    msg.type = MFS_LOOKUP_MSG;
    msg.pinum = pinum;
    strcpy(msg.name, name);
    if (MFS_Send_Receive(&msg, &reply) < 0) return -1;
    return reply.rc;
}

int MFS_Stat(int inum, MFS_Stat_t *m) {
    if (inum < 0 || m == NULL) return -1;
    mfs_msg_t msg;
    mfs_reply_t reply;
    msg.type = MFS_STAT_MSG;
    msg.inum = inum;
    if (MFS_Send_Receive(&msg, &reply) < 0) return -1;
    if (reply.rc < 0) return -1;
    *m = reply.stat;
    return 0;
}

int MFS_Write(int inum, char *buffer, int block) {
    if (inum < 0 || buffer == NULL || block < 0 || block >= 14) return -1;
    mfs_msg_t msg;
    mfs_reply_t reply;
    msg.type = MFS_WRITE_MSG;
    msg.inum = inum;
    msg.block = block;
    memcpy(msg.buffer, buffer, MFS_BLOCK_SIZE);
    if (MFS_Send_Receive(&msg, &reply) < 0) return -1;
    return reply.rc;
}

int MFS_Read(int inum, char *buffer, int block) {
    if (inum < 0 || buffer == NULL || block < 0 || block >= 14) return -1;
    mfs_msg_t msg;
    mfs_reply_t reply;
    msg.type = MFS_READ_MSG;
    msg.inum = inum;
    msg.block = block;
    if (MFS_Send_Receive(&msg, &reply) < 0) return -1;
    if (reply.rc < 0) return -1;
    memcpy(buffer, reply.buffer, MFS_BLOCK_SIZE);
    return 0;
}

int MFS_Creat(int pinum, int type, char *name) {
    if (pinum < 0 || name == NULL || strlen(name) >= 28) return -1;
    mfs_msg_t msg;
    mfs_reply_t reply;
    msg.type = MFS_CREAT_MSG;
    msg.pinum = pinum;
    msg.mfs_type = type;
    strcpy(msg.name, name);
    if (MFS_Send_Receive(&msg, &reply) < 0) return -1;
    return reply.rc;
}

int MFS_Unlink(int pinum, char *name) {
    if (pinum < 0 || name == NULL || strlen(name) >= 28) return -1;
    mfs_msg_t msg;
    mfs_reply_t reply;
    msg.type = MFS_UNLINK_MSG;
    msg.pinum = pinum;
    strcpy(msg.name, name);
    if (MFS_Send_Receive(&msg, &reply) < 0) return -1;
    return reply.rc;
}

int MFS_Shutdown() {
    mfs_msg_t msg;
    mfs_reply_t reply;
    msg.type = MFS_SHUTDOWN_MSG;
    if (MFS_Send_Receive(&msg, &reply) < 0) return -1;
    return reply.rc;
}
