# Initial UNIX Utilities

## Project Description and Core Concepts

The `initial-utilities` collection consists of simplified reimplementations of classic Unix command-line tools. These projects serve as the entry point into systems programming, focusing on the essential interaction between user-space applications and the Operating System kernel. Key concepts explored include:

- **File I/O Abstractions:** Working with standard input, standard output, and file pointers.
- **Buffer Management:** Reading and writing data in chunks to optimize system calls.
- **Process Exit States:** Communicating success or failure to the parent process via standard exit codes.

## Included Utilities

1.  **[wcat](./wcat/)**: A basic tool for reading and concatenating files.
2.  **[wgrep](./wgrep/)**: A pattern-matching utility for searching text within files or streams.
3.  **[wunzip](./wunzip/)**: A decompression tool for restoring RLE-encoded files.

## Features

- **Standard Unix Behavior:** Mimics the core interface of traditional tools like `cat` and `grep`.
- **Minimalist Design:** Focuses on clean, readable C code that emphasizes fundamental OS principles.
- **Reliable Error Handling:** Consistent approach to reporting file access and usage errors.

## Compilation/Build Instructions

Each utility is a standalone C program and can be compiled using `gcc`. For example:

```bash
cd wcat
gcc -Wall -Werror -o wcat wcat.c
```

## Usage Examples

Navigate to the specific utility directory for detailed examples. A general usage pattern for `wgrep` is:

```bash
./wgrep/wgrep "pattern" input.txt
```

---
*Part of the Operating Systems Projects collection.*
