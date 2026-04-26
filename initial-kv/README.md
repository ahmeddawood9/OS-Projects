# Persistent Key-Value Store (KV)

## Project Description and Core Concepts

`kv` is a lightweight, persistent key-value store implemented in C. It demonstrates the intersection of in-memory data structures and file-based persistence, common in database engine internals. Key concepts include:

- **Dynamic Data Structures:** Using a linked list to manage arbitrary numbers of key-value pairs in memory.
- **File Persistence:** Implementing a save/load mechanism to ensure data survives process termination.
- **String Parsing:** Efficiently processing comma-separated command strings using `strsep`.
- **Error Resilience:** Graceful handling of malformed commands and file I/O failures.

## Features

- **Standard Operations:**
  - `p,key,value`: **Put** (insert or update) a key-value pair.
  - `g,key`: **Get** the value associated with a key.
  - `d,key`: **Delete** a specific key.
  - `c`: **Clear** all entries.
  - `a`: **All** - Display every stored pair.
- **Persistent Storage:** Automatically maintains state in `database.txt`.
- **Batch Processing:** Supports multiple operations in a single command-line execution.

## Compilation/Build Instructions

A `Makefile` is provided for easy compilation.

To build the executable:
```bash
make
```

To clean build artifacts:
```bash
make clean
```

## Usage Examples

```bash
# Store multiple pairs
./kv p,1,apple p,2,banana

# Retrieve a value
./kv g,1

# List all entries
./kv a

# Delete an entry and clear the database
./kv d,1 c
```

---
*Part of the Operating Systems Projects collection.*
