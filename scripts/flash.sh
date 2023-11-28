#!/usr/bin/env bash

PROG_NAME=$1
./scripts/pico.py C bootsel
echo bootsel ^
sleep 5
cp ./build/bin/$PROG_NAME.uf2 /run/media/$USER/RPI-RP2
echo file uploaded ^
sleep 5
echo starting monitor...
./scripts/monitor.py /dev/ttyACM0