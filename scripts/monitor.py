#!/usr/bin/env python3

import serial
import time
import os

while not os.path.exists('/dev/ttyACM0'):
    time.sleep(1)  # Wait until the device is connected

# Continuously attempt to read from the device
while True:
    try:
        # Attempt to open the serial port
        with serial.Serial('/dev/ttyACM0', 9600, timeout=1) as ser:
            while True:
                if ser.in_waiting:
                    data = ser.readline().decode('utf-8').rstrip()
                    print(data)
                else:
                    time.sleep(0.1)  # Little delay to avoid constant polling
    except serial.SerialException:
        print("Device disconnected, attempting to reconnect...")
        time.sleep(1)  # Wait before attempting to reconnect

