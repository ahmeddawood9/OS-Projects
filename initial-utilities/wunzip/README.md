# wunzip - RLE Decompression Utility

`wunzip` is a custom file decompression tool written in C. It was developed as part of the "initial-utilities" project from the *Operating Systems: Three Easy Pieces* (OSTEP) curriculum.

## Description

`wunzip` is the counterpart to `wzip`. It reverses Run-Length Encoding (RLE) compression by reading a binary compressed file and restoring it to its original text format, which is then sent to standard output (`stdout`).

## How Decompression Works

The utility expects the specific binary format used by `wzip`:
1. **Count (4 bytes):** A binary integer representing the number of times a character is repeated.
2. **Character (1 byte):** The ASCII character to be repeated.

The tool reads these 5-byte chunks sequentially, printing the character to the terminal the specified number of times until the end of the file is reached.

## Implementation Details

The implementation focuses on safe binary file handling:
* **Binary Stream Processing:** Since compressed files may contain bytes that represent newlines (`0x0A`) as part of an integer value, standard text-reading functions (like `getline`) cannot be used.
* **Raw Memory Access:** The tool uses `fread()` to pull raw bytes directly into memory, ensuring that 4-byte integers and 1-byte characters are deserialized correctly regardless of their content.
* **Multi-file Support:** `wunzip` can process multiple files in a single execution, concatenating the decompressed output seamlessly.

## Compilation

To compile the project with strict error checking, use `gcc`:

```bash
gcc -o wunzip wunzip.c -Wall -Werror
```

## Usage

### Basic Decompression
To decompress a file and view the output in the terminal:
```bash
./wunzip compressed_file.z
```

### Restore Original File
To decompress and save the result back to a text file:
```bash
./wunzip compressed_file.z > restored.txt
```

### Batch Decompression
To decompress multiple files sequentially:
```bash
./wunzip file1.z file2.z
```

## Testing

A test script is included to verify the implementation against the OSTEP test suite:

```bash
./test-wunzip.sh
```
