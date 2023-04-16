cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 17)

# ---------- Version ----------
set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)
set(VERSION_PATCH 0)

# ---------- Setup  ----------
# Importing the pico-sdk importer
include(pico-sdk/pico_sdk_init.cmake)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
include(util)

# Project
project(CMS_Avionics_Code)
pico_sdk_init()

set(stdlibs pico_stdlib hardware_gpio hardware_spi)

# To make sure build folder doesn't get cluttered
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(
    -Wall # Enable all warnings
    -O3 # I am speed
    -Werror # Treat warnings as errors
)

if(PICO_PLATFORM MATCHES "host")
    add_compile_options(
        -Wno-error # host compilation redefines shit for some reason, and it prevents compilation
    )
endif()

# ---------- Libraries  ----------
subdirlist(libs ${CMAKE_SOURCE_DIR}/lib)

foreach(libname ${libs})
    file(GLOB_RECURSE srcs ${CMAKE_SOURCE_DIR}/lib/${libname}/*.c)
    file(GLOB_RECURSE hdrs ${CMAKE_SOURCE_DIR}/lib/${libname}/*.h)

    add_library(${libname} ${srcs} ${hdrs})
    target_link_libraries(${libname} ${stdlibs})

    # target_include_directories(${libname} PUBLIC ${CMAKE_SOURCE_DIR}/lib/${libname})
    include_directories(${CMAKE_SOURCE_DIR}/lib/${libname}/include)
endforeach()

# ---------- Boards  ----------
set(boards
    echo
)

foreach(board ${boards})
    file(GLOB_RECURSE srcs ${CMAKE_SOURCE_DIR}/src/${board}/*.c)
    file(GLOB_RECURSE hdrs ${CMAKE_SOURCE_DIR}/src/${board}/*.h)

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