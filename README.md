# Embedded Systems Refresher

A structured C/C++ and embedded-systems refresher focused on rebuilding practical firmware-development skills through progressively more advanced projects.

The repository progresses from hardware-independent C simulations to real-time firmware running on a Raspberry Pi Pico 2, with an eventual emphasis on biomedical-device development.

## Goals

* Refresh C programming fundamentals
* Develop embedded-C programming skills
* Refresh modern C++
* Practice microcontroller architecture and peripheral programming
* Develop firmware using simulated and physical hardware
* Practice real-time and concurrent programming concepts
* Develop experience with sensor acquisition and host-device communication
* Apply embedded-system principles to biomedical devices

## Projects

### Week 1 — C Foundations

A simulated physiological signal analyzer using 12-bit ADC samples.

Topics:

* Fixed-width integer types
* Arrays and pointers
* Pointer arithmetic
* Structs and memory alignment
* Integer overflow
* `const`
* Header/source separation
* Defensive input checking
* Compiler warnings
* Basic signal processing

See [`week01-c-foundations`](week01-c-foundations/).

### Week 2 — Firmware Integration

A simulated embedded acquisition system integrating several firmware components into a producer-consumer architecture.

Topics:

* `static`, `const`, and `volatile`
* Bit manipulation and register concepts
* Finite-state machines
* Simulated ADC acquisition
* Polling versus interrupts
* Circular buffers
* Single-producer/single-consumer ring buffers
* C11 atomics
* Acquire/release memory ordering
* Event flags and counters
* Overflow detection
* Buffer sizing
* High-water marks and hysteresis
* Producer-consumer timing

Experiments explored temporary processing stalls, sustained producer-consumer throughput mismatches, buffer overflow, and event-loss behavior.

See [`week02-firmware-integration`](week02-firmware-integration/).

### Week 3 — Raspberry Pi Pico 2 Hardware

Firmware concepts were transferred from simulation to a Raspberry Pi Pico 2.

Projects include:

#### GPIO and Interrupts

* Onboard LED control
* Push-button input with an internal pull-up
* Active-low GPIO logic
* Edge detection
* Mechanical-switch debouncing
* Non-blocking debounce
* GPIO interrupt callbacks
* Deferred processing outside interrupt context

#### Repeating Timers

Implemented periodic timer callbacks and explored:

* Periodic execution
* Fixed-rate 1 kHz events
* Interrupt/callback execution constraints
* Atomic event counting
* Separating time-critical callbacks from slower main-loop processing

#### ADC Acquisition

Connected a potentiometer to GPIO26 / ADC0 and acquired 12-bit ADC measurements across the 0–4095 digital range.

#### Real-Time ADC Ring Buffer

Implemented a 1 kHz producer-consumer acquisition pipeline:

```text
Repeating timer
      |
      v
  ADC read
      |
      v
SPSC ring buffer
      |
      v
  Main loop
      |
      v
Processing / diagnostics
```

The repeating-timer callback acts as the producer, while the main loop acts as the consumer.

Experiments demonstrated:

* Stable acquisition at approximately 1,000 samples per second
* Zero overflow when the consumer keeps pace with acquisition
* Buffer overflow when the consumer was intentionally limited to approximately 500 samples per second
* High-water monitoring before data loss occurs
* A temporary 14 ms consumer pause producing a 14-sample backlog while remaining within the 15-sample usable ring-buffer capacity
* The distinction between buffering temporary latency and attempting to compensate for a sustained throughput deficit

### Pico 2 ADC → USB → Python Streaming

Built an end-to-end real-time data acquisition pipeline:

- Samples the RP2350 ADC at 1 kHz using a repeating timer
- Transfers samples from the timer callback through an atomic SPSC ring buffer
- Assigns sequence numbers for end-to-end sample-loss detection
- Reports firmware ring-buffer overflows separately
- Streams structured records over USB CDC
- Parses the stream in Python using PySerial
- Maintains a rolling 2-second sample window
- Visualizes the ADC waveform live with Matplotlib at ~20 FPS

Serial protocol:

- `S,<sequence>,<adc_value>` — ADC sample
- `O,<overflow_count>` — firmware overflow diagnostic

Validation with a potentiometer produced approximately 1,000 samples/s with zero detected sequence losses and zero Pico ring-buffer overflows.

See [`week03-pico2-hardware`](week03-pico2-hardware/).

## Development Environment

Development is performed on macOS using Visual Studio Code, Clang, CMake, the Raspberry Pi Pico SDK, and a Raspberry Pi Pico 2.

Early exercises use hardware-independent simulations to develop and test firmware concepts before transferring those concepts to physical microcontroller hardware.

Current hardware includes:

* Raspberry Pi Pico 2
* Breadboard
* Push button
* Potentiometer
* Jumper wires
* MAX30102 pulse oximeter / heart-rate sensor module

USB CDC serial communication is used for host-side debugging and diagnostic output.

## Roadmap

Future exercises will cover:

* I2C communication with the MAX30102
* MAX30102 register configuration and FIFO acquisition
* Real-time red and infrared PPG waveform acquisition
* PPG signal filtering and peak detection
* Heart-rate estimation
* SpO2 estimation
* ADC and sensor sampling/timing analysis
* UART and SPI
* DMA
* Modern C++
* RTOS concepts
* Embedded signal processing
* Embedded machine learning
* Biomedical-device capstone project
