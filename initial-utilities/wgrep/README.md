# wgrep — A Simple grep-like Utility

`wgrep` is a small UNIX-style command-line tool written in **C** that mimics the core behavior of `grep`. It searches for a **case-sensitive substring** in text input and prints every line that contains the search term.

This project is built for learning **Operating Systems and systems programming fundamentals**.

---

## Features

* Case-sensitive substring search
* Supports **multiple files**
* Reads from **standard input (stdin)** if no file is provided
* Handles **arbitrarily long lines** using `getline()`
* Proper UNIX-style error handling and exit codes

---

## Build

```bash
gcc -Wall -Werror -o wgrep wgrep.c
```

---

## Usage

```bash
./wgrep searchterm [file ...]
```

### Examples

```bash
./wgrep foo bar.txt
```

```bash
cat file.txt | ./wgrep hello
```

---

## Errors & Exit Codes

* Missing arguments → prints usage message, exits with `1`
* Cannot open file → prints error message, exits with `1`
* Successful execution → exits with `0`

---


