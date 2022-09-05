set(EMU_SOURCES
    src/main.c
)

add_executable(emu ${EMU_SOURCES})

target_link_libraries(emu pico_stdlib)

pico_add_extra_outputs(emu)
