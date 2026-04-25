# Operating Systems Projects

This repository is a comprehensive collection of projects and experiments designed to explore and implement core Operating Systems concepts. Based on the "Operating Systems: Three Easy Pieces" (OSTEP) curriculum and real-world system monitoring, this collection bridges the gap between theoretical OS principles and hands-on systems programming.

## Project Description and Core Concepts

The goal of this repository is to provide a deep dive into the inner workings of modern operating systems. By implementing classic Unix utilities, a custom shell, and a system monitor, these projects cover the fundamental pillars of OS design:

- **Virtualization:** Process management, CPU scheduling, and memory abstraction.
- **Concurrency:** Multi-threaded execution and synchronization (via shell parallelization).
- **Persistence:** File I/O, file system structures, and data serialization.
- **System Observation:** Interacting with the Linux kernel via the `/proc` virtual filesystem.

## Repository Structure

The repository is organized into several focused sub-projects:

1.  **[Initial Utilities](./initial-utilities/)**: Reimplementing standard Unix tools (`wcat`, `wgrep`, `wunzip`) to master basic file I/O and memory management.
2.  **[Reverse Utility](./initial-reverse/)**: A tool for reversing file content, focusing on dynamic memory (linked lists) and robust error handling.
3.  **[WISH - The Wisconsin Shell](./Processes-shell/)**: A custom Unix-style shell featuring process forking, path resolution, and parallel execution.
4.  **[System Monitor](./system-monitor/)**: A lightweight Linux resource monitor that reads kernel state directly to visualize system health.

## Key Features

- **Systems Programming in C:** Rigorous use of C for low-level memory management and system calls.
- **Process Control:** Hands-on experience with `fork()`, `exec()`, and `wait()`.
- **Kernel Interaction:** Practical use of the `/proc` filesystem for real-time monitoring.
- **Automated Testing:** Integration with OSTEP-style testing frameworks to ensure reliability and correctness.
- **Professional Documentation:** Consistent and detailed guides for every sub-module.

## Compilation and Build Instructions

Most C-based projects in this repository include a `Makefile` or use standard `gcc` commands. Generally, you can build them using:

```bash
# Example for a C project
cd initial-reverse
make
```

Or manually:

```bash
gcc -Wall -Werror -o program_name source.c
```

Specific instructions for each tool are located in their respective directories.

## Usage Examples

Navigate to any sub-directory to find detailed usage examples. For instance, to use the `wgrep` utility:

```bash
./initial-utilities/wgrep/wgrep "search_term" input_file.txt
```

To start the system monitor:

```bash
cd system-monitor
./collector.sh
python -m http.server 8000
```

---
*Developed as part of an ongoing journey to master Operating Systems and Systems Programming.*
