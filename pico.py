#!/usr/bin/env python3

import RPi.GPIO as GPIO
from time import sleep
import argparse

GPIO.setmode(GPIO.BCM)

class PicoPort:
    def __init__(self, run, bootsel):
        self.run_pin = run
        self.bootsel_pin = bootsel

        # setup as output
        GPIO.setup(self.run_pin, GPIO.OUT)
        GPIO.setup(self.bootsel_pin, GPIO.OUT)

        # set initial state
        GPIO.output(self.run_pin, GPIO.HIGH)
        GPIO.output(self.bootsel_pin, GPIO.HIGH)

    def reset(self):
        GPIO.output(self.run_pin, GPIO.LOW)
        sleep(0.1)
        GPIO.output(self.bootsel_pin, GPIO.HIGH)

    def bootsel(self):
        GPIO.output(self.bootsel_pin, GPIO.LOW)
        sleep(0.1)
        self.reset()
        sleep(0.1)
        GPIO.output(self.bootsel_pin, GPIO.HIGH)

devices = {
    "A": PicoPort(15, 14),
    "B": PicoPort(12, 16)
}

if __name__ == "__main__":
    GPIO.setmode(GPIO.BCM)

    parser = argparse.ArgumentParser(description='Pico port control')
    parser.add_argument('device', choices=devices.keys(), help='device to control')
    parser.add_argument('action', choices=["reset", "bootsel"], help='action to perform')

    args = parser.parse_args()

    device = devices[args.device]
    if args.action == "reset":
        device.reset()
    elif args.action == "bootsel":
        device.bootsel()


