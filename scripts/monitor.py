#!/usr/bin/env python3

import serial
import time
import sys
import os

path = sys.argv[1]

# Continuously attempt to read from the device
while True:
    try:
        # Attempt to open the serial port
        with serial.Serial(path, 9600) as ser:
            while True:
                if ser.in_waiting:
                    data = ser.readline().decode('utf-8').rstrip()
                    print(data)
                else:
                    time.sleep(0)  # yield
    except KeyboardInterrupt:
        break
    except:
        print("Device disconnected, attempting to reconnect...")
        time.sleep(1)  # Wait before attempting to reconnect

