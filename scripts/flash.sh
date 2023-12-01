#!/usr/bin/env bash

make

PROG_NAME=$1
./scripts/pico.py C bootsel
echo bootsel ^
sleep 3
cp ./build/bin/$PROG_NAME.uf2 /run/media/$USER/RPI-RP2
echo file uploaded ^

echo starting monitor...
./scripts/monitor.py /dev/ttyACM0
