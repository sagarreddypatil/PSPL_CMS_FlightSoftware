cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)

# ---------- Version ----------
set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)
set(VERSION_PATCH 1)

# ---------- Pico SDK Project Setup  ----------
# Importing the pico-sdk importer
include(external/pico-sdk/pico_sdk_init.cmake)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "cmake/")

# Project
project(CMS_Avionics_Code)
pico_sdk_init()

set(stdlibs pico_stdlib hardware_gpio hardware_spi pico_mbedtls pico_bootsel_via_double_reset)

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
# target_link_libraries(mpack)
target_include_directories(mpack PUBLIC external/mpack/src/mpack)

# ---------- Libraries  ----------

set(libs)

macro(psp_lib libname)
    set(deps ${ARGN})

    file(GLOB_RECURSE srcs lib/${libname}/*.c)
    file(GLOB_RECURSE hdrs lib/${libname}/*.h)

    add_library(${libname} INTERFACE)
    target_sources(${libname} INTERFACE ${srcs})
    target_link_libraries(${libname} INTERFACE ${stdlibs} ${deps})

    target_include_directories(${libname} INTERFACE lib/${libname}/include)
    list(APPEND libs ${libname})

endmacro()

# this one is special
include_directories(lib/crypto/include)

psp_lib(crypto)
psp_lib(spi)
psp_lib(max31856 spi)
psp_lib(w5500 spi)
psp_lib(commandnet w5500 mpack)

# ---------- Boards  ----------
set(boards
    echo
    lotsofpackets
    emu
    sanity-check
)

foreach(board ${boards})
    file(GLOB_RECURSE srcs src/${board}/*.c)
    file(GLOB_RECURSE hdrs src/${board}/*.h)

    add_executable(${board} ${srcs} ${hdrs})
    pico_set_program_name(${board} "${board}")
    pico_set_program_version(${board} "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH} (build ${CMAKE_BUILD_TYPE})")

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
