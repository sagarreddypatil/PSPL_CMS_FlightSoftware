#!/usr/bin/env python3

import RPi.GPIO as GPIO
from time import sleep


WAIT=0.1

GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)

class Pico:
  def __init__(self, run, bootsel):
    self.run_pin = run
    self.bootsel_pin = bootsel

    GPIO.setup(run, GPIO.OUT)
    GPIO.setup(bootsel, GPIO.OUT)

    GPIO.output(run, GPIO.HIGH)
    GPIO.output(bootsel, GPIO.LOW)
  
  def reset(self):
    GPIO.output(self.run_pin, GPIO.LOW)
    sleep(WAIT)
    GPIO.output(self.run_pin, GPIO.HIGH)

  def bootsel(self):
    GPIO.output(self.bootsel_pin, GPIO.LOW)
    sleep(WAIT)
    self.reset()
    sleep(WAIT)
    GPIO.output(self.bootsel_pin, GPIO.HIGH)


devices = {
  "A": Pico(15, 14),
  "B": Pico(12, 16),
}

def act_reset(dev: Pico):
  dev.reset()

def act_bootsel(dev: Pico):
  dev.bootsel()

actions = {
  "reset": act_reset,
  "bootsel": act_bootsel,
}

import argparse

parser = argparse.ArgumentParser(description='Pico control')
parser.add_argument('device', choices=devices.keys(), help='device to control')
parser.add_argument('action', choices=actions.keys(), help='action to perform')

args = parser.parse_args()

device = devices[args.device]
action = actions[args.action]

action(device)
