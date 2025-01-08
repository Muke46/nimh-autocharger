import serial
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import threading
import time

# Serial port configuration
SERIAL_PORT = '/dev/ttyACM0'  # Replace with your port, e.g., 'COM3', '/dev/ttyUSB0'
BAUD_RATE = 9600

# Data storage
voltage_data = []
charger_status_data = []
time_data = []

# Function to read data from the serial port
def read_serial():
    global voltage_data, charger_status_data, time_data
    ser = serial.Serial(SERIAL_PORT, BAUD_RATE)
    while True:
        try:
            line = ser.readline().decode('utf-8').strip()  # Read a line from the serial port
            # Parse the data
            if line.startswith("Charging: Voltage"):
                parts = line.split(",")
                voltage = float(parts[0].split()[2].strip().replace("V", ""))
                charger_status = int(parts[1].split(":")[1].strip())
                # Append the data
                voltage_data.append(voltage)
                charger_status_data.append(charger_status)
                time_data.append(time.time())
                if len(voltage_data) > 100:  # Limit data points for better performance
                    voltage_data.pop(0)
                    charger_status_data.pop(0)
                    time_data.pop(0)
        except Exception as e:
            print(f"Error: {e}")
            break

# Function to update the plot
def update_plot(frame):
    # Clear the axes for live updates
    ax1.cla()
    ax2.cla()
    
    # Plot voltage on ax1
    ax1.plot(time_data, voltage_data, label="Battery Voltage (V)", color="blue")
    ax1.set_ylim(0, 1.6)
    ax1.set_ylabel("Voltage (V)", color="blue")
    ax1.set_xlabel("Time (s)")
    ax1.grid(True)
    ax1.legend(loc="upper left")
    
    # Plot charger status on ax2 (sharing x-axis with ax1)
    ax2.plot(time_data, charger_status_data, label="Charger Status (10-bit)", color="green", linestyle="--")
    ax2.set_ylim(0, 1023)
    ax2.set_ylabel("Charger Status (0-1023)", color="green")
    ax2.legend(loc="upper right")

# Start the serial reading thread
thread = threading.Thread(target=read_serial, daemon=True)
thread.start()

# Create the figure and axes for live plotting
fig, ax1 = plt.subplots()
ax2 = ax1.twinx()  # Create a twin y-axis for charger status

# Use FuncAnimation for live updating
ani = FuncAnimation(fig, update_plot, interval=1000)

plt.show()