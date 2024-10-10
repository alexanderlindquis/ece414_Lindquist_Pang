cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(ts_test)

pico_sdk_init()

add_executable(ts_test)

pico_generate_pio_header(ts_test ${CMAKE_CURRENT_LIST_DIR}/SPIPIO.pio)

target_sources(ts_test PRIVATE 
    ts_test.c
    TouchScreen.c
    TFTMaster.c
    glcdfont.c
    ts_lcd.c
    )

target_link_libraries(ts_test PRIVATE 
    pico_stdlib 
    hardware_adc 
    hardware_gpio hardware_pio 
    hardware_irq    
    pico_bootsel_via_double_reset
    )

pico_add_extra_outputs(ts_test)






