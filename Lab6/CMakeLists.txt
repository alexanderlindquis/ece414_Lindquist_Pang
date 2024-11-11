cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(PID)

pico_sdk_init()

add_executable(PID
        )

<<<<<<< HEAD
pico_generate_pio_header(PID
        ${CMAKE_CURRENT_LIST_DIR}/SPIPIO.pio)
=======
pico_generate_pio_header(PID ${CMAKE_CURRENT_LIST_DIR}/SPIPIO.pio)
>>>>>>> 8a7e469995f4c22f99e1a86db6c5ac6c54816c2f

target_sources(PID PRIVATE 
        PID.c
<<<<<<< HEAD
        
=======
>>>>>>> 8a7e469995f4c22f99e1a86db6c5ac6c54816c2f
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
<<<<<<< HEAD
=======
        hardware_irq
>>>>>>> 8a7e469995f4c22f99e1a86db6c5ac6c54816c2f
        pico_bootsel_via_double_reset
        )

# create map/bin/hex file etc.
pico_add_extra_outputs(PID)

