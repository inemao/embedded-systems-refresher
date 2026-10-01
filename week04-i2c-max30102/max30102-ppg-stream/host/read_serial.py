import serial
import time
import matplotlib.pyplot as plt
from collections import deque

PORT = "/dev/cu.usbmodem1101"
BAUD_RATE = 115200
SAMPLE_RATE_HZ = 100
WINDOW_SECONDS = 5

ser = serial.Serial(PORT, BAUD_RATE, timeout=1)
red_data = deque(
    maxlen=SAMPLE_RATE_HZ * WINDOW_SECONDS
)
ir_data = deque(
    maxlen=SAMPLE_RATE_HZ * WINDOW_SECONDS
)

sample_count = 0
start_time = time.time()

last_sequence = None


total_sequence_losses = 0
total_sensor_overflows = 0

plt.ion()

fig, ax = plt.subplots()
red_line, = ax.plot([], [], label="Red")
ir_line, = ax.plot([], [], label="IR")

ax.set_xlabel("Time (s)")
ax.set_ylabel("ADC counts")
ax.legend()

PLOT_INTERVAL_S = 1.0 / 20.0
last_plot_time = time.time()

try:
    while True:

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

            sample_count += 1
            red_data.append(red)
            ir_data.append(ir)
            # sequence-loss detection
            if last_sequence is not None:
                lost = sequence -  last_sequence - 1
            
                if lost > 0:
                    total_sequence_losses += lost
            
            last_sequence = sequence

        elif parts[0] == "O":
            sensor_overflows = int(parts[1])
            total_sensor_overflows += sensor_overflows

        now = time.time()
        elapsed_time = now - start_time


        
        
        
        if now - last_plot_time >= PLOT_INTERVAL_S:
            time_data = [
                i / SAMPLE_RATE_HZ
                for i in range(len(red_data))
            ]

            red_line.set_data(time_data, red_data)
            ir_line.set_data(time_data, ir_data)

            ax.relim()
            ax.autoscale_view()

            fig.canvas.draw()
            fig.canvas.flush_events()
            plt.pause(0.001)

            last_plot_time = now


        if elapsed_time >= 1:
            print(f"Received/s: {sample_count} | Total sequence losses: {total_sequence_losses} | Total sensor overflows: {total_sensor_overflows}")
            start_time = now
            sample_count = 0
finally:        
    ser.close()