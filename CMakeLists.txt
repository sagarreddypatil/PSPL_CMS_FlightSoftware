cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)

# ---------- Version ----------
# set(VERSION_MAJOR 0)
# set(VERSION_MINOR 2)
# set(VERSION_PATCH 0)

# ---------- Pico SDK Project Setup  ----------
# Importing the pico-sdk importer
include(external/pico-sdk/pico_sdk_init.cmake)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "cmake/")

# Project
project(CMS_Avionics_Code)
pico_sdk_init()

set(stdlibs pico_stdlib hardware_gpio hardware_adc hardware_spi pico_bootsel_via_double_reset)

# To make sure build folder doesn't get cluttered
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(
    -Wall # Enable all warnings
    -O3 # I am speed
    -Werror # Treat warnings as errors
    -Wno-unused-variable # annoying, don't keep it tho
    -Wno-strict-aliasing # pico-sdk fails this
)

if(PICO_PLATFORM MATCHES "host")
    add_compile_options(
        -Wno-error # host compilation redefines shit for some reason, and it prevents compilation
    )
endif()

# ---------- MPack  ----------

set(MPACK_STDIO 0)
set(MPACK_FLOAT 0)
set(MPACK_DOUBLE 0)

file(GLOB_RECURSE mpack_srcs external/mpack/src/mpack/*.c)
file(GLOB_RECURSE mpack_hdrs external/mpack/src/mpack/*.h)

add_library(mpack ${mpack_srcs} ${mpack_hdrs})
target_include_directories(mpack PUBLIC external/mpack/src/mpack)

# --------- Git Describe ---------
execute_process(
    COMMAND git describe --tags --dirty
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_DESCRIBE_STRING
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# ---------- Libraries  ----------

set(libs
    mpack
)

macro(psp_lib loc libname)
    file(GLOB_RECURSE srcs ${loc}/${libname}/*.c)
    file(GLOB_RECURSE hdrs ${loc}/${libname}/*.h)

    add_library(${libname} INTERFACE)
    target_sources(${libname} INTERFACE ${srcs})
    target_link_libraries(${libname} INTERFACE ${stdlibs} ${libs})

    target_include_directories(${libname} INTERFACE ${loc}/${libname}/include)
    list(APPEND libs ${libname})
endmacro()

# this one is special
include_directories(lib/crypto/include)

# list these in topoligical order
psp_lib(device spi)
psp_lib(device max31856)
psp_lib(device w5500)
psp_lib(device ads13x)
psp_lib(device w25n01)

psp_lib(lib util)
psp_lib(lib crypto)
psp_lib(lib sensornet)
psp_lib(lib commandnet)
psp_lib(lib ntp)

# ---------- Boards  ----------
set(boards
    # flash-test
    # adc-test
    # echo
    ntp-test
    emu
    # tc-test-board
    # sanity-check
)

foreach(board ${boards})
    file(GLOB_RECURSE srcs src/${board}/*.c)
    file(GLOB_RECURSE hdrs src/${board}/*.h)

    add_executable(${board} ${srcs} ${hdrs})
    pico_set_program_name(${board} "${board}")
    pico_set_program_version(${board} "${GIT_DESCRIBE_STRING} (build ${CMAKE_BUILD_TYPE})")

    set_target_properties(${board}
        PROPERTIES

        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/elf
    )

    target_link_libraries(${board} ${stdlibs} ${libs})

    pico_enable_stdio_usb(${board} 1)
    pico_enable_stdio_uart(${board} 0)

    pico_set_binary_type(${board} copy_to_ram) # run full program from ram

    # pico_set_float_implementation(${board} none)
    pico_add_uf2_output(${board})

endforeach()
