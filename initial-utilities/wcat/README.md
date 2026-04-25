# wcat - OS Utility

## Project Description and Core Concepts

`wcat` is a simplified version of the Unix `cat` command, implemented in C. This project serves as an introduction to basic systems programming and the Linux system call interface. Core concepts include:

- **File Descriptor Management:** Opening and closing files using `fopen` and `fclose`.
- **Buffered I/O:** Reading data efficiently from disk and writing it to standard output (`stdout`).
- **Error Handling in C:** Detecting and reporting issues such as missing files or permission errors.

## Features

- **Multiple File Support:** Processes and displays the contents of one or more files sequentially.
- **Robust Error Handling:** Provides descriptive error messages if a file cannot be opened.
- **Standardized Return Codes:** Follows Unix conventions for success (0) and failure (1) exit codes.

## Compilation/Build Instructions

The utility can be compiled using `gcc` with standard flags.

To build the executable:
```bash
gcc -Wall -Werror -o wcat wcat.c
```

## Usage Examples

### 1. View a Single File
Print the contents of a text file to the terminal:
```bash
./wcat file.txt
```

### 2. Concatenate Multiple Files
Print the contents of multiple files in sequence:
```bash
./wcat file1.txt file2.txt file3.txt
```

---
*Part of the Operating Systems Projects collection.*
