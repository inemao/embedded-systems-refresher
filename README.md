# Embedded Systems Refresher

A structured C/C++ and embedded-systems refresher focused on rebuilding
practical firmware-development skills through progressively more advanced
projects.

## Goals

- Refresh C programming fundamentals
- Develop embedded-C programming skills
- Refresh modern C++
- Practice microcontroller architecture and peripheral programming
- Develop firmware using simulated hardware
- Practice real-time and concurrent programming concepts
- Apply embedded-system principles to biomedical devices

## Projects

### Week 1 — C Foundations

A simulated physiological signal analyzer using 12-bit ADC samples.

Topics:

- Fixed-width integer types
- Arrays and pointers
- Pointer arithmetic
- Structs and memory alignment
- Integer overflow
- `const`
- Header/source separation
- Defensive input checking
- Compiler warnings
- Basic signal processing

See [`week01-c-foundations`](week01-c-foundations/).

## Development Environment

Development is currently performed on macOS using Clang. Hardware-independent
projects are used initially so that the fundamentals can be practiced without
requiring a physical microcontroller.

## Roadmap

Future exercises will cover:

- Embedded C
- Bit manipulation and hardware registers
- `volatile` and memory-mapped I/O
- Circular buffers
- Finite-state machines
- Microcontroller peripherals
- Interrupts
- UART, SPI, and I2C
- Modern C++
- RTOS concepts
- Embedded biomedical-device design
