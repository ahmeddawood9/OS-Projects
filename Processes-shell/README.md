# WISH - The Wisconsin Shell

## Project Description and Core Concepts

WISH (Wisconsin Shell) is a custom Unix-style command-line interpreter (CLI) developed in C. This project serves as a deep dive into the process management and execution model of an Operating System. Key concepts demonstrated include:

- **Process Creation and Control:** Leveraging `fork()`, `execv()`, and `wait()` to manage the lifecycle of child processes.
- **I/O Redirection:** Implementing the `>` operator to redirect standard output and standard error from the terminal to files.
- **Path Resolution:** Dynamically searching for executables within a user-defined set of directories.
- **Parallelism:** Managing multiple concurrent processes using the `&` operator to achieve asynchronous command execution.

## Features

- **Interactive and Batch Modes:** Supports real-time command entry via a REPL loop or automated execution from a script file.
- **Built-in Commands:** 
  - `exit`: Gracefully terminates the shell.
  - `cd`: Changes the current working directory.
  - `path`: Configures the search directories for executables.
- **Output Redirection:** Seamlessly sends command output to files.
- **Parallel Execution:** Runs multiple commands simultaneously, waiting for all to complete before returning to the prompt.
- **Robust Parsing:** Handles complex input strings, including whitespace variations and multiple operators.

## Compilation/Build Instructions

The shell is designed to be compiled with `gcc` using strict flags to ensure code quality and stability.

To compile the shell:
```bash
gcc wish.c -o wish -Wall -Werror
```

## Usage Examples

### 1. Interactive Mode
Launch the shell and enter commands at the `wish>` prompt:
```bash
./wish
wish> ls -la
wish> cd /tmp
wish> exit
```

### 2. Batch Mode
Run a script file containing a sequence of commands:
```bash
./wish script.sh
```

### 3. Redirection
Redirect the output of a command to a file:
```bash
wish> ls /usr/bin > output.txt
```

### 4. Parallel Commands
Execute multiple programs concurrently:
```bash
wish> ls & ps & whoami
```

---
*Part of the Operating Systems Projects collection.*
