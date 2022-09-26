cmake_minimum_required(VERSION 3.13)

# ---------- Setup  ----------
# to make sure stuff in the cmake folder can be located
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

# Importing the pico-sdk importer
set(PICO_SDK_FETCH_FROM_GIT on)
include(pico_sdk_import)

# General utility macros
include(util)

# Project
project(CMS_Avionics_Code)
pico_sdk_init()

set(stdlibs pico_stdlib hardware_gpio hardware_spi)

# To make sure build folder doesn't get cluttered
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

add_compile_options(
    -Wall # Enable all warnings
    -Werror # Treat warnings as errors
    -O3 # I am speed
)

# ---------- Libraries  ----------
subdirlist(libs ${CMAKE_SOURCE_DIR}/lib)

foreach(libname ${libs})
    file(GLOB_RECURSE srcs ${CMAKE_SOURCE_DIR}/lib/${libname}/*.c)
    file(GLOB_RECURSE hdrs ${CMAKE_SOURCE_DIR}/lib/${libname}/*.h)

    add_library(${libname} ${srcs} ${hdrs})
    target_link_libraries(${libname} ${stdlibs})

    # target_include_directories(${libname} PUBLIC ${CMAKE_SOURCE_DIR}/lib/${libname})
    include_directories(${CMAKE_SOURCE_DIR}/lib/${libname}) # this is kinda dangerous, libraries cannot depend on each other, but can reference headers
endforeach()

# ---------- Boards  ----------
set(boards
    emu
)

foreach(board ${boards})
    file(GLOB_RECURSE srcs ${CMAKE_SOURCE_DIR}/${board}/*.c)
    file(GLOB_RECURSE hdrs ${CMAKE_SOURCE_DIR}/${board}/*.h)

    add_executable(${board} ${srcs} ${hdrs})
    target_link_libraries(${board} ${stdlibs} ${libs})

    pico_enable_stdio_usb(${board} 1)
    pico_enable_stdio_uart(${board} 0)

    pico_add_uf2_output(${board})

    set_target_properties(${board}
        PROPERTIES

        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${board}
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${board}
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${board}
    )
endforeach()