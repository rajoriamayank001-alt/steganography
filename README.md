# Image Steganography Using LSB Technique

## Project Overview
This project implements Image Steganography using the Least Significant Bit (LSB) technique in C programming.

Steganography is a technique used to hide secret information inside an image without making noticeable changes to its appearance.

## Features
- Encode secret files into BMP images.
- Decode hidden secret files from stego images.
- Uses the LSB (Least Significant Bit) technique.
- Supports command-line arguments.
- Validates input files and image capacity.

## Technologies Used
- C Programming
- File Handling
- Bitwise Operations
- Pointers
- Structures
- Command Line Arguments
- BMP Image Format

## Project Files
- `main.c` – Main program.
- `encode.c` – Encoding functionality.
- `encode.h` – Encoding declarations.
- `decode.c` – Decoding functionality.
- `decode.h` – Decoding declarations.
- `common.h` – Common definitions.
- `types.h` – Data types and status definitions.
- `test_encode.c` – Encoding test file.
- `test_decode.c` – Decoding test file.

## Working Principle
The LSB technique hides secret data by modifying the least significant bit of image pixel data.

The encoding process stores secret information inside the BMP image.

The decoding process retrieves the hidden information from the encoded image.

## Author

Mayank Rajoria

GitHub: https://github.com/rajoriamayank001-alt

## Compilation

Compile the project using GCC:

```bash
gcc main.c encode.c decode.c -o stego
```

## Execution

### Encoding

```bash
./stego -e beautiful.bmp secret.txt stego.bmp
```

### Decoding

```bash
./stego -d stego.bmp output.txt
```

## Learning Outcomes

- Understanding LSB Steganography.
- File handling in C.
- Bitwise operations.
- Pointers and structures.
- Command Line Arguments.
- BMP image processing.