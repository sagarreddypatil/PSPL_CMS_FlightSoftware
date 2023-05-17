# Flight Software for Crater Maker Special

## Overview
 - MCU: RP2040
 - RTOS: None
 - HAL: pico-sdk

## Filesystem Hirearchy
```
PSPL_CMS_Avionics_Code/
├── README.md <- This file
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
├── external/ <- Gitignored, for automatically downloaded libraries(in Makefile), contains pico-sdk
├── include/ <- Global include directory, used for configs and stuff
├── build/ <- not uploaded to the repo, contains compilation outputs
│   └── bin/ <- contains the files to be uploaded to the boards
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
This repo uses Git submodules, so you need to clone it with the `--recursive` flag.
```
git clone --recurse-submodules https://url/to/repo
```

If you already cloned the repo, you can run `git submodule update --init --recursive` to get the submodules.

### Working on a project
All Setup and Development Operations information is on Confluence.

## Building
Run `make build` for a debug build, or `make release` for a release build.
`make clean` will delete the build folder.

Output binaries for each board will be present in `build/uf2`