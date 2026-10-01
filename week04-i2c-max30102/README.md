# Week 4: I2C and MAX30102 PPG Acquisition

## Overview

Register-level implementation of a two-channel photoplethysmography
(PPG) acquisition system using the Raspberry Pi Pico 2 and MAX30102.

The project progresses from basic I2C communication to continuous
Red/IR optical data acquisition and live Python visualization.

## Hardware

- Raspberry Pi Pico 2
- MAX30102 breakout board
- USB connection to host computer

### Wiring

| MAX30102 | Pico 2 |
|----------|--------|
| VIN | 3V3 OUT (pin 36) |
| GND | GND (pin 8) |
| SDA | GP4 (pin 6) |
| SCL | GP5 (pin 7) |

I2C address: 0x57

## Project Progression

1. `i2c-device-scan/` - Detect devices on the I2C bus.
2. `max30102-part-id/` - Read and verify the MAX30102 part ID.
3. `max30102-registers/` - Configure acquisition registers.
4. `max30102-fifo/` - Read and reconstruct 18-bit Red/IR samples.
5. `max30102-ppg-stream/` - Continuous FIFO acquisition and USB streaming.

## Acquisition Configuration

- Sampling rate: nominally 100 samples/second
- ADC resolution: 18 bits
- Red and IR acquisition
- Six bytes per FIFO sample
- Hardware FIFO overflow monitoring

## Serial Protocol

Sample record:

S,<sequence>,<red>,<ir>

Sensor FIFO overflow record:

O,<count>

Sequence numbers allow the host application to identify gaps in
received sample records.

## Python Visualization

The host application uses PySerial and Matplotlib to:

- Receive and parse Red/IR measurements.
- Maintain a rolling five-second sample window.
- Display both channels at approximately 20 frames/second.
- Track sequence gaps and sensor FIFO overflows.

Run from the `max30102-ppg-stream/host/` directory:

    python3 -m pip install pyserial matplotlib
    python3 read_serial.py

Update the serial port in the script for your computer.

## Current Status

Continuous two-channel optical acquisition and live visualization
have been demonstrated.

Initial streaming tests showed no detected sequence losses or
sensor FIFO overflows.

## Next Steps

- Validate the effective acquisition rate.
- Separate AC and DC signal components.
- Characterize pulse morphology.
- Implement pulse detection and heart-rate estimation.
- Investigate physiological calibration and validation.

This is an engineering development project, not a validated
medical device.
