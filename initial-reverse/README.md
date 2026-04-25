# Reverse Utility

## Project Description and Core Concepts

`reverse` is a lightweight Unix command-line utility written in C that reads an input stream and prints its lines in reverse order (Last-In, First-Out). This project is a practical exercise in systems programming, focusing on fundamental Operating Systems concepts:

- **Manual Memory Management:** Utilizing `malloc`, `free`, and dynamic data structures (linked lists) to handle files of arbitrary size.
- **File System Interaction:** Manipulation of file streams and validation of system-level metadata (inodes) to prevent data corruption.
- **LIFO Data Structures:** Implementation of a stack-like mechanism to reverse line order efficiently.

## Features

- **Dynamic Line Handling:** Successfully processes files with an arbitrary number of lines and varying line lengths.
- **Flexible I/O Modes:** 
  - Reads from `stdin` or a user-specified file.
  - Writes to `stdout` or a user-specified file.
- **Robust Error Handling:** 
  - Validates argument counts and file accessibility.
  - Prevents overwriting the input file with the output file by comparing device and inode numbers.
  - Gracefully handles memory allocation failures.

## Compilation/Build Instructions

A `Makefile` is provided for streamlined compilation. The program is built using `gcc` with strict warning flags.

To build the executable:
```bash
make
```

To remove build artifacts:
```bash
make clean
```

Alternatively, you can compile manually:
```bash
gcc -Wall -Werror -o reverse reverse.c
```

## Usage Examples

The utility adapts its behavior based on the number of command-line arguments provided:

### 1. Interactive Mode (Standard Input to Standard Output)
Read lines from the terminal and print them in reverse. Press `Ctrl+D` to signal EOF.
```bash
./reverse
```

### 2. File to Screen
Read lines from a file and print them to the terminal.
```bash
./reverse input.txt
```

### 3. File to File
Read lines from an input file and write the reversed output to a separate output file.
```bash
./reverse input.txt output.txt
```

---
*Part of the Operating Systems Projects collection.*
