cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(main)

pico_sdk_init()

add_executable(main)

pico_generate_pio_header(main ${CMAKE_CURRENT_LIST_DIR}/SPIPIO.pio)

target_sources(main PRIVATE 
    main.c
    calculator.c
    TouchScreen.c
    TFTMaster.c
    glcdfont.c
    ts_lcd.c
    interface.c
    timer.c
    )

target_link_libraries(main PRIVATE 
    pico_stdlib 
    hardware_adc 
    hardware_gpio hardware_pio 
    hardware_irq    
    pico_bootsel_via_double_reset
    )

pico_add_extra_outputs(main)






