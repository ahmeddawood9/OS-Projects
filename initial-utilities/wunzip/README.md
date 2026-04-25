# wunzip - RLE Decompression Utility

## Project Description and Core Concepts

`wunzip` is a specialized file decompression tool written in C that reverses Run-Length Encoding (RLE). This project focuses on binary file manipulation and raw data processing, which are critical for understanding how Operating Systems handle non-textual data. Key concepts include:

- **Binary I/O:** Using `fread` to read fixed-width binary structures (4-byte integers and 1-byte characters) rather than line-based text.
- **Data Deserialization:** Correctly interpreting raw bytes into their original program-level representations.
- **Lossless Decompression:** Restoring compressed data to its exact original state without data loss.

## Features

- **Efficient RLE Decoding:** Decodes 5-byte chunks (4-byte count + 1-byte char) to reconstruct original text.
- **Binary Stream Safety:** Correctly handles non-printable characters and binary values that might be mistaken for control characters in text mode.
- **Multi-file Concatenation:** Seamlessly decompresses multiple files into a single unified output stream.

## Compilation/Build Instructions

The utility is compiled using `gcc` with standard systems programming flags.

To build the executable:
```bash
gcc -Wall -Werror -o wunzip wunzip.c
```

## Usage Examples

### 1. Basic Decompression
Decompress a file and view the output in the terminal:
```bash
./wunzip compressed_file.z
```

### 2. Restore to File
Decompress a file and save the output to a new text file using redirection:
```bash
./wunzip compressed_file.z > original.txt
```

### 3. Batch Processing
Decompress multiple files in sequence:
```bash
./wunzip file1.z file2.z file3.z
```

---
*Part of the Operating Systems Projects collection.*
