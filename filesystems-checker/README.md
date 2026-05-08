# xv6 File System Checker (xcheck)

This project implements a file system checker for the xv6 operating system. The checker reads an xv6 file system image and ensures its internal consistency by performing several checks on inodes, data blocks, directories, and bitmaps.

## Features

The checker (`xcheck`) performs the following 12 consistency checks:

1.  **Inode Type Validation:** Each inode must be either unallocated or one of the valid types (`T_FILE`, `T_DIR`, `T_DEV`).
2.  **Block Address Validation:** All direct and indirect block addresses used by in-use inodes must point to valid data blocks within the image.
3.  **Root Directory Check:** The root directory (inode 1) must exist, and its parent must be itself.
4.  **Directory Formatting:** Each directory must contain `.` and `..` entries, with `.` pointing to the directory itself.
5.  **Bitmap Consistency (Used Blocks):** Every block address used by an inode must be marked as used in the bitmap.
6.  **Bitmap Consistency (Free Blocks):** Every block marked as used in the bitmap must actually be in use by an inode.
7.  **Direct Block Uniqueness:** Each direct block address must be used by at most one inode.
8.  **Indirect Block Uniqueness:** Each indirect block address must be used by at most one inode.
9.  **Inode Reference Check:** Every in-use inode must be referred to in at least one directory.
10. **Directory Reference Check:** Every inode referred to in a directory must be marked as in-use.
11. **File Link Count:** The `nlink` count for regular files must match the number of times they are referred to in directories.
12. **Directory Link Count:** Each directory can appear in at most one other directory (no hard links to directories).

### Contest Features (Advanced)

This implementation also includes:
- **Parent Directory Mismatch Check:** Ensures `..` entries correctly point to the parent.
- **Loop Detection:** Detects inaccessible directories and cycles in the directory tree.
- **Repair Mode (`-r`):** Automatically recovers "lost" inodes (marked use but not found in any directory) by linking them to the `lost_found` directory.

## Compilation

To compile the checker, run:

```bash
make
```

## Usage

To run the checker on a file system image:

```bash
./xcheck <file_system_image>
```

To run in repair mode (requires a `lost_found` directory in the image):

```bash
./xcheck -r <file_system_image>
```

## Exit Codes

- `0`: No consistency errors found (or all errors repaired in `-r` mode).
- `1`: A consistency error was detected, or the image could not be opened.

## License

Part of the OSTEP (Operating Systems: Three Easy Pieces) projects collection.
