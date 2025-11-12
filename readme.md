# LSB Steganography

A command-line tool that hides secret data inside BMP images using the Least Significant Bit technique.

## Description

This tool encodes secret text files into BMP images by manipulating the least significant bits of pixel values, making the changes invisible to the human eye. It supports both encoding and decoding operations with automatic file extension preservation.

## Features

- Hide text and code files (.txt, .c, .sh, .h) inside BMP images
- Extract hidden data from steganographic images
- Automatic capacity validation before encoding
- File extension preservation during operations
- Magic string verification for data integrity

## Installation

Clone the repository:
```bash
git clone https://github.com/ArjunVasavan/lsb-steganography.git
cd lsb-steganography
```

## Compilation

```bash
gcc *.c -o steganography
```

## Usage

### Encoding (Hide Data)

```bash
# With custom output file
./steganography -e source.bmp secret.txt output.bmp

# With default output (creates default.bmp)
./steganography -e source.bmp secret.txt
```

### Decoding (Extract Data)

```bash
# With custom output file
./steganography -d encoded.bmp decoded.txt

# With default output (auto-detects extension)
./steganography -d encoded.bmp
```

## How It Works

The program uses LSB (Least Significant Bit) steganography to embed data into BMP images. It encodes:
1. Magic string for verification
2. File extension length and name
3. Secret data size
4. The actual secret data

Each byte of data is distributed across 8 bytes of image data by modifying only the least significant bit of each pixel component.

## Author

Arjun Vasavan

## License

This project is open source and available for educational purposes.
