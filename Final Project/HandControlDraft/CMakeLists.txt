cmake_minimum_required(VERSION 3.13)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(hand_controller_test VERSION 1.0)
pico_sdk_init()

# Create executable
add_executable(hand_controller_test
    src/main.c
    src/lsm6dsox.c
    src/display.c
    src/TFTMaster.c
    src/SPIPIO.c
)

# Generate PIO header
pico_generate_pio_header(hand_controller_test ${CMAKE_CURRENT_LIST_DIR}/src/SPIPIO.pio)

target_link_libraries(hand_controller_test 
    pico_stdlib
    hardware_i2c
    hardware_pio
    hardware_pwm
)

# Enable USB output, disable UART output
pico_enable_stdio_usb(hand_controller_test 1)
pico_enable_stdio_uart(hand_controller_test 0)

# Create map/bin/hex/uf2 file
pico_add_extra_outputs(hand_controller_test)

# Add include directory
target_include_directories(hand_controller_test PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/include
)