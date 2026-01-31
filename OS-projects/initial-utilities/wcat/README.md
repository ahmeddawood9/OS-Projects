# wcat — OS Utility

A simplified implementation of the Unix `cat` command.

## Description
This program reads one or more files and prints their contents
to standard output. Implemented as part of an Operating Systems
course .

## Features
- Supports multiple files
- Proper error handling
- Correct exit codes
- Buffered file I/O

## Build
```bash
gcc -Wall -Wextra -o wcat wcat.c
