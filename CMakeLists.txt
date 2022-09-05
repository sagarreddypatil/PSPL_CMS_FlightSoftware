cmake_minimum_required(VERSION 3.13)

include(pico-sdk/pico_sdk_init.cmake)

project(cms_flightsoftware)
set(CMAKE_C_STANDARD 17)

set(CMS_FLIGHTSOFTWARE_PATH ${PROJECT_SOURCE_DIR})

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

add_compile_options(-Wall # copied from example
    -Wno-format # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
    -Wno-unused-function # we have some for the docs that aren't called
    -Wno-maybe-uninitialized
)

add_subdirectory(blink)
add_subdirectory(emu)