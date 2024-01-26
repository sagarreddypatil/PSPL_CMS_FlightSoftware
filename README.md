# Flight Software for Crater Maker Special

## Overview

- MCU: RP2040
- RTOS: FreeRTOS
- HAL: pico-sdk

## Branches

Information as of Aug 28, 2023, bother Sagar if you want it updated

- `main`: development branch, contains latest (mostly) tested code, broken code
  is not allowed, POC is Sagar Patil
- `cmdnet-secure`: encrypted version of CommandNet, POC is Sagar Patil,
  information on Confluence
- `ads13x`: driver development for ADS13xMXX ADCs, POC is Mihir Patil
- `lte-modem`: LTE modem driver, POC is Jay Jagani
- `stuff_on_pi`: random stuff from Taylor's Pi, should probably merge it, POC is
  Sagar Patil
- `freertos`: exploratory branch for FreeRTOS, POC is Sagar Patil and Charlie Schoettle
- `vn200`: driver for the VN200 IMU, POC is Charlie Schoettle
- `tests`: working test framework, but dead branch, POC is Sagar Patil
- `host-compilation`: dead branch, host compilation for testing purposes, POC is
  Jay Jagani (?)
- `camerons-stuff`: Dead branch, old code written for the ATMega328p, archival,
  POC is Cameron Williams

## Filesystem Hirearchy

```
PSPL_CMS_Avionics_Code/
├── README.md <- This file
│
│
├── src/ <- contains main programs for each board
│   └── someprogram/ <- Main program folder, i.e., for a board
│       ├── someprogram.h <- All header stuff for this program
│       ├── main.c <- Contains the entrypoint
│       └── other.c <- Other source files
├── lib/ <- common libraries shared by all boards
│   └── example/ <- example library
│       ├── example.c
│       └── include/ <- header files, added to include path
│           ├── example.h <- top level header, included with `#include <example.h>`
│           └── example/ <- secondary headers
│               └── whatever.h <- included with `#include <example/whatever.h>`
├── device/ <- Same structure as lib, but for device drivers
│
│
├── external/ <- Gitignored, for automatically downloaded libraries(in Makefile), contains pico-sdk
├── build/ <- not uploaded to the repo, contains compilation outputs
│   └── bin/ <- contains the files to be uploaded to the boards
│
│
├── .clang-format <- file containing autoformatter rules
├── .gitignore <- file containing things to not be uploaded to GitHub(e.g., exe's, build folder)
├── Doxyfile <- Doxygen Documentation Generator Configs, mostly autogenerated
├── CMakeLists.txt <- Main build script
└── Makefile <- User-friendly build script, calls CMake
```

## Usage

### Dependencies

- `git` (to clone this repo)
- `make` (for running CMake)
- `ninja` (CMake backend)
- `cmake` (build system)
- `compdb` (compile database for headers, optional)
- `gcc-arm-none-eabi` (compiler)
- `ccache` (compiler cache, optional)
- `newlib-arm-none-eabi` (C standard library)

### Cloning this Repo

Just clone it like you normally would

### Working on a project

All Setup and Development Operations information is on Confluence.

## Building

Run `make` or `make build` for a debug build, or `make release` for a release build.
`make clean` will delete the build folder.

Output binaries for each board will be present in `build/bin/`. ELF binaries can
be found in `build/bin/elf/`

## Remote Dev on Bang Bang Boom Box

### Overview

- Raspberry Pi 4, running Raspberry Pi OS
- 0-2 Pi Picos connected at any time, ask in #avionics for which boards are
  connected
  - Connected over USB
  - BOOTSEL (puts the Pico into a flashable state) and RUN (reset) connected to
    Pi's GPIO

There's Pico A and Pico B, each with a set of bootsel and reset pins.

You can see how many Pico's are connected by running `lsusb`

### Dependencies

- Zerotier VPN
- SSH
- VSCode (optional)

### Setup

- Install Zerotier, ask Sagar for the network ID
- Join the network
- SSH into the Pi

**VSCode Setup**

- Install the Remote SSH extension
- Use the extension to connect to the Pi

Flight software is located in `~/Documents/FlightSoftware_CMS`

### Flashing Code

There are scripts under `scripts/` that can be used to flash code to the Pi.
They are:

- `flash.sh`: Copy the UF2 file to the Pico (this flashes the Pico)
- `pico.py`: See later, this can be used to bootsel and reset the Pico
- `router.sh`: (Legacy, do not execute) Used to setup the Pi as a router

1. Decide which Pico you want to use (A or B)
1. Run `./scripts/pico.py <A/B> bootsel`
   - Example: `./scripts/pico.py A bootsel`
   - This will put the Pico into a flashable state
   - You can verify this by running `lsusb`, you'll see that one of the Pico's
     is listed as "Raspberry Pi Pico Boot" or something similar
1. Run `./scripts/flash.sh <board-name>`
   - Example: `./scripts/flash.sh ntp-test`
   - This will flash the Pico
1. To see the output, run `cat /dev/ttyACM0`
   - If multiple Picos are connected, use `ls /dev/ttyACM*` to list all the
     output ports.
   - Your Pico can be either `/dev/ttyACM0` or `/dev/ttyACM1`, try both

If at any point you want to reset the Pico, run `./scripts/pico.py <A/B> reset`

### FAQ

**Q: I'm getting a `Permission Denied` error when running `./scripts/pico.py` or**
**`./scripts/flash.sh`**

A: Wait a bit longer, the Pico is still booting up. If it's been more than 30
seconds, ask Sagar.

**Q: Sagar isn't responding, what do I do?**

A: If waiting doesn't fix the permission problem, try this

1. Reset both Picos (doesn't matter if they exist or not)
   - `./scripts/pico.py A reset`
   - `./scripts/pico.py B reset`
1. Run `sudo rm /media/pspl/RPI-RP2*`
   - This deletes any stray mount points left behind, which is probably what's
     causing the script to fail

**Q: There aren't any Pico's listed when I run `lsusb`**

A: Reboot the Raspberry Pi by running `sudo reboot`. Exit your terminal, and
wait a bit for the Pi to reboot. Then SSH back in and try again.

**Q: That didn't work, what do I do?**

A: Contact Sagar and Taylor on the #avionics channel on Slack.
