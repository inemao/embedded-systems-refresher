import serial
import time
import matplotlib.pyplot as plt
from collections import deque

PORT = "/dev/cu.usbmodem1101"
BAUD_RATE = 115200
SAMPLE_RATE_HZ = 1000
WINDOW_SECONDS = 2

ser = serial.Serial(PORT, BAUD_RATE, timeout=1)
samples = deque(
    maxlen=SAMPLE_RATE_HZ * WINDOW_SECONDS
)

sample_count = 0
start_time = time.time()

previous_sequence = None
lost_count = 0

total_sequence_losses = 0
total_pico_overflows = 0

plt.ion()

fig, ax = plt.subplots()
line_plot, = ax.plot([])

ax.set_ylim(0, 4095)
ax.set_xlim(0, WINDOW_SECONDS)
ax.set_xlabel("Time (seconds)")
ax.set_ylabel("ADC value")
ax.set_title("Pico 2 ADC — Live")

last_plot_time = time.time()
try:
    while True:

        line = ser.readline()

        decoded = line.decode()
        cleaned = decoded.strip()
        parts = cleaned.split(",")

        parts = cleaned.split(",")

    

        if parts[0] == "S":
            sequence = int(parts[1])
            sample = int(parts[2])

            sample_count += 1
            samples.append(sample)
            # sequence-loss detection

        elif parts[0] == "O":
            pico_overflows = int(parts[1])
            total_pico_overflows += pico_overflows

        current_time = time.time()
        plot_elapsed = current_time - last_plot_time
        elapsed_time = current_time - start_time


        if plot_elapsed >= 0.05:
            line_plot.set_data(
                [i / SAMPLE_RATE_HZ for i in range(len(samples))],
                samples
            )

            fig.canvas.draw()
            fig.canvas.flush_events()
            plt.pause(0.001)

            last_plot_time = current_time


        if previous_sequence is not None:
            lost = sequence -  previous_sequence - 1

            if lost > 0:
                total_sequence_losses += lost

        previous_sequence = sequence

        if elapsed_time >= 1:
            print(f"Received/s: {sample_count} | Total sequence losses: {total_sequence_losses} | Total Pico overflows: {total_pico_overflows}")
            start_time = current_time
            sample_count = 0
finally:        
    ser.close()