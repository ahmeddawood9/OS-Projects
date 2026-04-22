# WISH - The Wisconsin Shell

WISH (Wisconsin Shell) is a simple Unix command-line interpreter (CLI) developed in C. This project explores the core fundamentals of operating systems, including process creation, memory management, and I/O redirection.

## Features

- **Interactive Mode**: A classic REPL loop providing a `wish>` prompt for real-time command execution.
- **Batch Mode**: Ability to execute commands from a script file (e.g., `./wish batch.txt`).
- **Path Searching**: Dynamic search path for executables (defaults to `/bin`).
- **Built-in Commands**:
  - `exit`: Safely terminates the shell.
  - `cd`: Changes the current working directory.
  - `path`: Updates the search path for the shell.
- **Redirection**: Supports standard output and standard error redirection to files using the `>` operator.
- **Parallel Execution**: Ability to run multiple commands simultaneously using the `&` operator.

## Compilation

To compile the shell, use the following `gcc` command. It is configured to treat all warnings as errors to ensure high-quality, stable code.

```bash
gcc wish.c -o wish -Wall -Werror
```

## Usage

### Interactive Mode

Run the shell without arguments to enter interactive mode:

```bash
./wish
wish> ls -la
wish> cd /tmp
wish> exit
```

### Batch Mode

Pass a file containing a list of commands to run them sequentially:

```bash
./wish script.sh
```

### Parallel Commands

Run multiple programs at once:

```bash
wish> ls & ps & whoami
```

### Redirection

Send the output of a command to a file:

```bash
wish> ls /usr/bin > output.txt
```

## Technical Architecture

The shell operates through a cycle of three main stages:

1. **Parsing**: Using `strsep` to tokenize user input and identify operators like `&` and `>`.
2. **Execution**: Using `fork()` to create child processes and `execv()` to replace the child process image with the target executable.
3. **Synchronization**: Using `waitpid()` to manage the lifecycle of child processes, ensuring parallel tasks finish before returning control to the user.
