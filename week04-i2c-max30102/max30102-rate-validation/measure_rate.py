import serial
import time

# Serial configuration
PORT = "/dev/cu.usbmodem1101"
BAUD_RATE = 115200

# Measurement configuration
RATE_WINDOW_S = 30.0

# Open serial connection
ser = serial.Serial(PORT, BAUD_RATE, timeout=1)

# Initialize measurement state
rate_start = time.monotonic()
rate_samples = 0

last_sequence = None
total_sequence_losses = 0
total_sensor_overflows = 0

try:
    while True:

        # Read one line from the Pico
        line = ser.readline()
        decoded = line.decode()
        cleaned = decoded.strip()

        if not cleaned:
            continue

        parts = cleaned.split(",")

        if parts[0] == "S":

            sequence = int(parts[1])
            red = int(parts[2])
            ir = int(parts[3])

            # Count received sample records
            rate_samples += 1

            # Detect sequence discontinuities
            if last_sequence is not None:
                lost = sequence - last_sequence - 1

                if lost > 0:
                    total_sequence_losses += lost

            last_sequence = sequence

        elif parts[0] == "O":

            sensor_overflows = int(parts[1])
            total_sensor_overflows += sensor_overflows

          # Check measurement window
        now = time.monotonic()
        elapsed = now - rate_start

        if elapsed >= RATE_WINDOW_S:

            measured_rate = rate_samples / elapsed

            print(
                f"Average rate: {measured_rate:.3f} samples/s | "
                f"Samples: {rate_samples} | "
                f"Elapsed: {elapsed:.2f} s | "
                f"Sequence losses: {total_sequence_losses} | "
                f"Sensor overflows: {total_sensor_overflows}"
            )

            # Reset measurement window
            rate_start = now
            rate_samples = 0

finally:
    ser.close()