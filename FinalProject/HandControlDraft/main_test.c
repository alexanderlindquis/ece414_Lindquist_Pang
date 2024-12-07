// main_test.c
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "lsm6dsox.h"
#include "display.h"
#include "sensor_fusion.h"

// Pin definitions
#define I2C_SDA 19
#define I2C_SCL 20
#define UPDATE_RATE_MS 20

int main() {
    stdio_init_all();
    
    // Initialize I2C
    i2c_init(i2c0, 400000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    
    printf("Initializing LSM6DSOX...\n");
    if (!lsm6dsox_init(i2c0)) {
        printf("Failed to initialize LSM6DSOX\n");
        return -1;
    }

    printf("Initializing Sensor Fusion...\n");
    sensor_fusion_init();
    
    printf("Initializing Display...\n");
    display_init();
    
    // Variables for data collection
    lsm6dsox_data_t raw_data;
    sensor_fusion_data_t fusion_data;
    display_data_t display_data = {
        .pitch = 0,
        .roll = 0,
        .car_speed = 0,
        .movement_state = DISPLAY_STOPPED,
        .connected = false
    };
    
    absolute_time_t last_update = get_absolute_time();
    bool sensor_ok = true;
    
    printf("Starting main loop...\n");
    while (1) {
        if (absolute_time_diff_us(last_update, get_absolute_time()) >= UPDATE_RATE_MS * 1000) {
            // Read and process sensor data
            if (lsm6dsox_read_data(&raw_data)) {
                if (sensor_fusion_update(&fusion_data, &raw_data)) {
                    // Calculate motion state based on angles
                    float abs_pitch = fabsf(fusion_data.pitch);
                    float abs_roll = fabsf(fusion_data.roll);
                    
                    // Handle forward/backward motion
                    if (abs_pitch > abs_roll) {
                        if (abs_pitch > 5.0f) { // Minimum tilt threshold
                            // Check for half-speed zone (45±10 degrees)
                            if (abs_pitch >= 35.0f && abs_pitch <= 55.0f) {
                                display_data.car_speed = 50; // Half speed
                            } else {
                                display_data.car_speed = (int)(abs_pitch * 100.0f / 90.0f);
                                if (display_data.car_speed > 100) display_data.car_speed = 100;
                            }
                            
                            display_data.movement_state = (fusion_data.pitch > 0) ? 
                                DISPLAY_FORWARD : DISPLAY_BACKWARD;
                        } else {
                            display_data.movement_state = DISPLAY_STOPPED;
                            display_data.car_speed = 0;
                        }
                    }
                    // Handle turning
                    else {
                        if (abs_roll > 15.0f) { // Minimum turn threshold
                            // Sharp turn at 60 degrees
                            if (abs_roll >= 60.0f) {
                                display_data.car_speed = 100;
                            } else {
                                display_data.car_speed = (int)(abs_roll * 100.0f / 60.0f);
                            }
                            
                            display_data.movement_state = (fusion_data.roll > 0) ? 
                                DISPLAY_TURNING_RIGHT : DISPLAY_TURNING_LEFT;
                        } else {
                            display_data.movement_state = DISPLAY_STOPPED;
                            display_data.car_speed = 0;
                        }
                    }
                    
                    // Update display
                    display_data.pitch = fusion_data.pitch;
                    display_data.roll = fusion_data.roll;
                    display_update(&display_data);
                    
                    // Debug output
                    printf("Pitch: %.2f, Roll: %.2f, Speed: %d, State: %d\n",
                           fusion_data.pitch, fusion_data.roll, 
                           display_data.car_speed, display_data.movement_state);
                } else {
                    printf("Sensor fusion error!\n");
                    display_data.movement_state = DISPLAY_STOPPED;
                    display_data.car_speed = 0;
                }
            } else {
                printf("Sensor read failed!\n");
                sensor_ok = false;
            }
            
            last_update = get_absolute_time();
        }
        
        sleep_ms(1);
    }

    return 0;
}