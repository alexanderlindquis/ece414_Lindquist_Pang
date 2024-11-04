cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(ic_pio_demo_project)

pico_sdk_init()

add_executable(PID
        )

pico_generate_pio_header(PID
        ${CMAKE_CURRENT_LIST_DIR}/ic_pio.pio )

        target_sources(PID PRIVATE 
        PID.c
        controller.c
        pwm_pin.c
        TouchScreen.c
        TFTMaster.c
        glcdfont.c
        ic_pio.c
        timer.c
        )
# pull in common dependencies
target_link_libraries(
        PID 
        pico_stdlib
        hardware_pio
        pico_bootsel_via_double_reset
        )

# create map/bin/hex file etc.
pico_add_extra_outputs(PID)

