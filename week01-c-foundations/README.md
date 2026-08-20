# Week 1 — C Foundations

This project is part of my C/C++ and embedded-systems refresher.

The program simulates a simple physiological signal acquisition workflow using
12-bit ADC samples and performs basic signal-processing operations.

## Features

- Simulates 12-bit ADC samples
- Assigns timestamps at a 100 Hz sampling rate
- Computes minimum and maximum values
- Computes the arithmetic mean
- Counts samples above a threshold
- Performs min-max normalization

## Concepts Practiced

- Fixed-width integer types
- Arrays and pointers
- Pointer-based function interfaces
- Structs
- `const`
- Integer overflow considerations
- Explicit type conversions
- Header/source separation
- Input validation
- Compiler warnings
- Basic signal processing

## Project Structure

```text
.
├── README.md
├── Makefile
├── .gitignore
├── include/
│   └── signal.h
└── src/
    ├── main.c
    └── signal.c
