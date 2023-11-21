#!/usr/bin/env bash

PROG_NAME=$1
./scripts/pico.py C bootsel
sleep 3
cp ./build/bin/$PROG_NAME.uf2 /run/media/$USER/RPI-RP2
sleep 3
./scripts/monitor.py /dev/ttyACM0