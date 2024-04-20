#!/usr/bin/env bash

PROG_NAME=$1

make
./scripts/pico.py A bootsel

echo
echo
echo RP2040 in bootsel mode...
echo -ne "\rsleeping... 3"
sleep 1
echo -ne "\rsleeping... 2"
sleep 1
echo -ne "\rsleeping... 1"
sleep 1
echo -ne "\rsleeping...  "
cp ./build/bin/$PROG_NAME.uf2 /run/media/$USER/RPI-RP2
echo -ne "\rFile uploaded!"
echo -ne "\rStarting monitor..."
echo -e "\n\n"
./scripts/monitor.py /dev/ttyACM0
