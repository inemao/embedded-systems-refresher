# MAX30102 Acquisition Rate Validation

## Objective
Measure host-observed sample throughput and monitor
acquisition integrity over consecutive 30-second windows.

## Test Configuration
- Hardware: Raspberry Pi Pico 2 + MAX30102
- Sensor configuration: Nominal 100 SPS
- Communication: I2C and USB CDC
- Host: Python with PySerial
- Measurement clock: time.monotonic()

## Results

| Metric | Window 1 | Window 2 |
|--------|----------|----------|
| Samples | 3116 | 3115 |
| Duration (s) | 30.01 | 30.01 |
| Observed rate (Hz) | 103.834 | 103.815 |
| Sequence losses | 0 | 0 |
| Sensor FIFO overflows | 0 | 0 |

## Interpretation
The observed throughput was approximately 103.82
samples per second, compared with a nominal configuration
of 100 SPS.

No sequence discontinuities or reported sensor FIFO
overflows were observed during the experiment.

The measured host throughput does not independently
establish the sensor's internal sampling frequency.

## Future Work
Investigate the discrepancy between nominal and
observed acquisition rates.