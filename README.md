# Initial KV Store

A simple persistent key-value store implemented in C.

## Features

- **Put**: `p,key,value` - Stores a key-value pair.
- **Get**: `g,key` - Retrieves the value for a given key.
- **Delete**: `d,key` - Removes a key-value pair.
- **Clear**: `c` - Removes all key-value pairs.
- **All**: `a` - Prints all key-value pairs.
- **Persistence**: Data is stored in `database.txt` and persists across sessions.

## Compilation

```bash
gcc -o kv kv.c -Wall -Werror
```

## Usage

```bash
./kv p,1,hello p,2,world
./kv g,1
./kv a
./kv d,1
./kv c
```

## Implementation Details

- Uses a **Linked List** for in-memory storage.
- Parses command-line arguments using `strsep`.
- Persists data to a plain-text file `database.txt` on exit and loads it on startup.
- Handles malformed commands gracefully by printing `bad command`.
