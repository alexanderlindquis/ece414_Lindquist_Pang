cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(calculator)

pico_sdk_init()

add_executable(calculator)

pico_generate_pio_header(calculator ${CMAKE_CURRENT_LIST_DIR}/SPIPIO.pio)

target_sources(calculator PRIVATE 
    calculator.c
    TouchScreen.c
    TFTMaster.c
    glcdfont.c
    ts_lcd.c
    Interface.c
    )

target_link_libraries(calculator PRIVATE 
    pico_stdlib 
    hardware_adc 
    hardware_gpio hardware_pio 
    hardware_irq    
    pico_bootsel_via_double_reset
    )

pico_add_extra_outputs(calculator)






