# wgrep - A Simple grep-like Utility

## Project Description and Core Concepts

`wgrep` is a custom command-line tool written in C that mimics the core functionality of the Unix `grep` utility. It searches for a specific substring within a stream of text and outputs every line that contains a match. This project explores:

- **Stream-based Processing:** Efficiently reading lines from both files and standard input (`stdin`).
- **Dynamic Line Buffering:** Utilizing `getline()` to handle lines of arbitrary length without buffer overflows.
- **Pattern Matching:** Implementing basic substring search within character arrays.

## Features

- **Multi-source Search:** Searches through multiple specified files or reads from `stdin` if no files are provided.
- **Case-sensitive Matching:** Performs precise substring matching on every line.
- **Memory Safety:** Dynamically allocates memory for line reading, ensuring robustness against unusually long input.
- **Error Validation:** Includes checks for missing search terms and inaccessible files.

## Compilation/Build Instructions

Compile the utility using `gcc`. The `-Werror` flag is recommended to ensure code quality.

To build the executable:
```bash
gcc -Wall -Werror -o wgrep wgrep.c
```

## Usage Examples

### 1. Search in a File
Find all lines containing "apple" in `fruits.txt`:
```bash
./wgrep apple fruits.txt
```

### 2. Search Multiple Files
Search across several files simultaneously:
```bash
./wgrep "error code" log1.txt log2.txt
```

### 3. Piping from Stdin
Filter output from another command:
```bash
cat file.txt | ./wgrep "hello"
```

---
*Part of the Operating Systems Projects collection.*
