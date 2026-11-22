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

### MAX30102 — Interrupt-Driven Acquisition with FreeRTOS

Implemented an interrupt-driven biomedical sensor acquisition pipeline using the Raspberry Pi Pico 2 (RP2350), MAX30102 optical sensor, and FreeRTOS V11.3.3.

The project demonstrates register-level peripheral communication, interrupt-to-task synchronization, priority-based scheduling, and intertask communication.

#### System Architecture

```text
MAX30102 optical sensor
    |  Red / IR measurements at 100 Hz
    |  Data-ready interrupt
    v
RP2350 GPIO interrupt handler
    |  Direct-to-task notification
    v
FreeRTOS acquisition task (priority 2)
    |  Read MAX30102 FIFO over I2C
    |  Send samples using xQueueSend()
    v
FreeRTOS queue (32 samples)
    |  xQueueReceive()
    v
FreeRTOS processing task (priority 1)
    |  Consume samples
    v
PPG signal-processing pipeline (planned)
```

A separate heartbeat task runs at priority 1 to provide a basic indication that the scheduler continues operating.

#### Implementation

- Register-level I2C communication with the MAX30102.
- Red and infrared optical measurements configured at 100 samples per second.
- GPIO interrupt handler using FreeRTOS direct-to-task notifications.
- Acquisition task that reads available samples from the sensor FIFO.
- Producer–consumer communication through a 32-element FreeRTOS queue.
- Nonblocking queue sends with overflow counting.
- Blocking queue receives to avoid unnecessary polling.
- Priority-based scheduling with acquisition at priority 2 and processing at priority 1.

#### Engineering Design Decisions

**Interrupt-driven acquisition:** The acquisition task blocks while waiting for sensor events, avoiding continuous polling when no new data is available.

**Task priorities:** Acquisition has higher priority than processing to reduce the risk of sensor FIFO overflow. The processing task consumes buffered samples when scheduled.

**Queue-based communication:** A FreeRTOS queue separates sensor acquisition from downstream processing. This allows signal-processing algorithms to evolve without tightly coupling them to the sensor driver.

**Nonblocking queue sends:** Acquisition does not wait for the processing task when the queue is full. Instead, the implementation counts unsuccessful queue sends.

#### Preliminary Validation

Initial hardware testing demonstrated:

- Sensor acquisition and processing operating at the configured 100 Hz sample rate.
- Processing sample counts keeping pace with acquisition counts during observed test intervals.
- A maximum observed producer-side queue depth of one sample under the tested workload.
- Continued heartbeat output during acquisition and processing.
- Task notification blocking intervals of approximately 6–7 RTOS ticks during diagnostic testing.

These observations establish basic functional operation under the tested conditions. They do not yet establish worst-case scheduling latency, long-duration reliability, or guaranteed lossless acquisition.

#### Next Steps

- Implement EMA-based PPG baseline estimation and AC extraction.
- Characterize filter behavior using Z-transforms and pole-zero analysis.
- Implement and validate heart-rate estimation.
- Compare EMA, Butterworth IIR, and FIR filtering approaches.
- Measure execution time, memory usage, and scheduling latency.
- Investigate PPG waveform morphology and physiological feature extraction.

This is an educational engineering prototype, not a clinically validated pulse oximeter.

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

### Completed

- C/C++ fundamentals and embedded programming exercises
- GPIO interrupts, timers, and ADC acquisition
- SPSC ring buffers and atomic synchronization
- Pico 2 hardware integration
- USB CDC streaming and Python visualization
- Register-level I2C communication with the MAX30102
- MAX30102 configuration and FIFO acquisition
- Interrupt-driven Red/IR PPG acquisition
- FreeRTOS integration on the RP2350
- Direct-to-task notifications
- Priority-based task scheduling
- Producer-consumer communication using FreeRTOS queues

### In Progress

- PPG signal processing
- EMA baseline estimation and AC extraction
- Z-transform and pole-zero analysis
- Heart-rate estimation

### Planned

- Butterworth IIR and FIR filter design
- Filter performance comparisons
- PPG waveform morphology analysis
- Execution-time and scheduling-latency measurements
- Stack and memory usage analysis
- SpO2 estimation and calibration considerations
- UART, SPI, and DMA
- Embedded machine learning
- Biomedical-device capstone project