#!/usr/bin/env bash

PROG_NAME=$1
./scripts/pico.py C bootsel
sleep 5
cp ./build/bin/$PROG_NAME.uf2 /run/media/$USER/RPI-RP2
sleep 5
./scripts/monitor.py /dev/ttyACM0