# Operating Systems Projects

This repository contains a collection of systems programming projects focusing on core Operating Systems concepts. Each project is implemented in C or Bash and explores fundamental topics such as process management, memory management, file systems, and concurrency.

## 📁 Repository Structure

| Project | Description | Key Concepts |
|:---|:---|:---|
| **[initial-utilities](./initial-utilities/)** | Reimplementation of `cat`, `grep`, `zip/unzip`. | File I/O, Buffer Management, System Calls. |
| **[initial-reverse](./initial-reverse/)** | A utility to reverse lines in a file or stream. | Linked Lists, Memory Management, File Metadata (Inodes). |
| **[initial-kv](./initial-kv/)** | A persistent key-value store. | Data Persistence, Command Parsing, Dynamic Memory. |
| **[Processes-shell](./Processes-shell/)** | WISH: The Wisconsin Shell. | `fork()`, `exec()`, `wait()`, I/O Redirection, Parallelism. |
| **[system-monitor](./system-monitor/)** | Real-time Linux resource monitor. | `/proc` filesystem, Kernel state extraction, Visualization. |
| **[tester](./tester/)** | Shared testing infrastructure. | Automated validation, Regression testing. |

## 🚀 Getting Started

### Prerequisites

- A Linux environment (recommended) or WSL.
- GCC compiler (`build-essential`).
- Bash shell.

### General Build Instructions

Most C-based projects can be compiled using the provided `Makefile` or a simple `gcc` command. For example, to build the `reverse` utility:

```bash
cd initial-reverse
make
```

### Running Tests

To verify the implementation of a project, you can use the `tester` scripts. Each project directory typically contains a `tests/` folder or a test script.

Example for `initial-reverse`:
```bash
cd initial-reverse
./test-reverse.sh
```

## 🛠 Features

- **Standard Compliance:** Adheres to common Unix utility behaviors.
- **Robustness:** Includes comprehensive error handling for file access, memory allocation, and user input.
- **Portability:** Designed for POSIX-compliant systems.
- **Automated Testing:** Integrated with a custom testing framework for reliability.

---
*Developed as part of an Operating Systems curriculum.*
