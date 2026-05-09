# Distributed File System (MFS)

This project implements a simple distributed file system based on a UDP communication protocol and a log-structured file system (LFS) design.

## Components

### 1. Server (`server.c`)
The server manages an on-disk file system image and handles requests from clients. 
- **Log-Structured File System:** All updates (data blocks, inodes, and imap pieces) are appended to the end of the log.
- **Checkpoint Region:** Maintains pointers to the pieces of the inode map and the current end of the log.
- **Inode Map:** Maps inode numbers to their disk addresses.
- **Idempotency:** The server ensures that all changes are committed to disk (via `fsync()`) before replying to the client.

### 2. Client Library (`libmfs.so`)
A shared library that provides a high-level API for applications to interact with the file server.
- **UDP Communication:** Uses a simple UDP wrapper to send and receive messages.
- **Reliability:** Implements timeouts and retries using `select()` to handle packet loss or server crashes.
- **API:** Supports functions like `MFS_Lookup`, `MFS_Stat`, `MFS_Read`, `MFS_Write`, `MFS_Creat`, and `MFS_Unlink`.

### 3. UDP Helper (`udp.c`, `udp.h`)
Provides basic abstraction for opening UDP sockets, reading, and writing packets.

### 4. Protocol (`mfs_proto.h`)
Defines the request and response structures used for communication between the client and server.

## Building the Project
To build the server and the client library, run:
```bash
make
```

## Running the Server
```bash
./server [portnum] [file-system-image]
```
If the image file does not exist, the server will initialize it with a root directory.

## Testing
A test program `client_test.c` is provided to verify the functionality.
To compile and run the test:
```bash
gcc -Wall -Werror -o client_test client_test.c -L. -lmfs -Wl,-rpath,.
./server 10000 fs_image &
./client_test
```

## Design Details
- **Max Inodes:** 4096
- **Block Size:** 4KB
- **Max File Size:** 56KB (14 direct pointers)
- **Directory Entries:** Fixed-length (32 bytes: 28 bytes name, 4 bytes inode number).
- **Initialization:** The root directory (inode 0) is created automatically on first run.
