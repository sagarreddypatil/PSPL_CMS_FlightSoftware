# Flight Software for Crater Maker Special

## Microcontroller
### RP2040

## Filesystem Hirearchy
```
Code
|  README.md <- this file
|--emu <- project folder, this one's the Engine Modulation Unit
|  |--src <- Main code folder
|     |  main.c <- Main script
|     |  example.c <- small auxillary script, doesn't actually exist
|  |--include <- Header files for axuillary scripts in the src folder
|     |  example.h <- Header file for example.c, also doesn't exist
|--lib <- Contains common libraries shared by all/some projects
|  |--example <- actually exists lmao
|     |--example.c
|     |--example.h
```

## Usage
### Working on a project
All Setup and Development Operations information is on Conlfuence.

## Building
Run `./build.sh` on Linux/macOS.
