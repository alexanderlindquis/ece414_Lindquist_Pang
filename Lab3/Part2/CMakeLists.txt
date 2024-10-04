cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(PONG1D)

pico_sdk_init()

add_executable(PONG1D
        PONG1D.c
        sw_in.c
        led_out.c
        debounce_sw1.c
        debounce_sw2.c
        timer.c
        )

# pull in common dependencies
target_link_libraries(
        PONG1D
        pico_stdlib
        pico_bootsel_via_double_reset
)

# create map/bin/hex file etc.
pico_add_extra_outputs(PONG1D)

