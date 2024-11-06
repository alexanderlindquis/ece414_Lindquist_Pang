cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(PID)

pico_sdk_init()

add_executable(PID
        )

pico_generate_pio_header(PID ${CMAKE_CURRENT_LIST_DIR}/SPIPIO.pio)

target_sources(PID PRIVATE 
        PID.c
        pwm_pin.c
        TouchScreen.c
        TFTMaster.c
        glcdfont.c
        ic_pio.c
        timer.c
        )
# pull in common dependencies
target_link_libraries(
        PID PRIVATE
        pico_stdlib
        pico_rand
        hardware_gpio
        hardware_pio
        hardware_pwm
        hardware_adc
        hardware_irq
        pico_bootsel_via_double_reset
        )

# create map/bin/hex file etc.
pico_add_extra_outputs(PID)

