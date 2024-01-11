#!/usr/bin/env python3

from lockfile import create_lockfile, delete_lockfile, check_lockfile

import serial
import time
import sys
import os


path = sys.argv[1]
file_name = os.path.basename(path)
output_dir = os.path.join("/tmp", "monitor_outputs")

if not os.path.exists(output_dir):
    os.makedirs(output_dir)

output_location = os.path.join(output_dir, file_name)
lockfile_location = output_location + ".lock"

if check_lockfile(lockfile_location):
    # lockfile exists, just start f"cat ${output_location}" instead
    os.execl("/bin/tail", "tail", "-f", output_location)


create_lockfile(lockfile_location)
outfile = open(output_location, "w+")

# Continuously attempt to read from the device
while True:
    try:
        # Attempt to open the serial port
        with serial.Serial(path, 9600) as ser:
            while True:
                if ser.in_waiting:
                    data = ser.readline().decode("utf-8").rstrip()
                    outfile.write(data + "\n")
                    outfile.flush()
                    print(data)
                else:
                    time.sleep(0)  # yield
    except KeyboardInterrupt:
        break
    except:
        message = "Device disconnected, attempting to reconnect..."
<<<<<<< Updated upstream
        outfile.write(message + "\n")
        print(message)

=======
        outfile.write()
        print(messag)
>>>>>>> Stashed changes
        try:
            time.sleep(1)  # Wait before attempting to reconnect
        except KeyboardInterrupt:
            break

outfile.close()
delete_lockfile(lockfile_location)
