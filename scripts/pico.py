#!/usr/bin/env python3

from time import sleep
import argparse
import os


class GPIO:
    OUT = "op"
    IN = "ip"
    HIGH = "dh"
    LOW = "dl"

    BCM = "lmao"
    BOARD = "lmao"

    _COMMAND = "raspi-gpio set"

    @staticmethod
    def run_command(pin, opt):
        command = f"{GPIO._COMMAND} {pin} {opt}"
        print(command)
        os.system(command)

    @staticmethod
    def setup(pin, mode):
        assert mode in [GPIO.IN, GPIO.OUT]

        GPIO.run_command(pin, mode)

    @staticmethod
    def output(pin, value):
        # TODO: check pin is setup as output
        assert value in [GPIO.HIGH, GPIO.LOW]

        GPIO.run_command(pin, value)

    @staticmethod
    def setmode(mode):
        pass  # TODO: i don't care, implement if you want


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
        GPIO.output(self.run_pin, GPIO.HIGH)
        sleep(0.1)

    def bootsel(self):
        GPIO.output(self.bootsel_pin, GPIO.LOW)
        sleep(0.1)

        self.reset()

        GPIO.output(self.bootsel_pin, GPIO.HIGH)


devices = {"A": (15, 14), "B": (12, 16)}

if __name__ == "__main__":
    GPIO.setmode(GPIO.BCM)

    parser = argparse.ArgumentParser(description="Pico port control")
    parser.add_argument(
        "device", choices=devices.keys(), help="device to control"
    )
    parser.add_argument(
        "action", choices=["reset", "bootsel"], help="action to perform"
    )

    args = parser.parse_args()

    device_pins = devices[args.device]
    device = PicoPort(*device_pins)
    if args.action == "reset":
        device.reset()
    elif args.action == "bootsel":
        device.bootsel()
