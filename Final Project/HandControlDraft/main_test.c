#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "lsm6dsox.h"
#include "display.h"

#define I2C_SDA 4
#define I2C_SCL 5
#define UPDATE_RATE_MS 100

int main() {
    stdio_init_all();
    
    // Initialize I2C for LSM6DSOX
    i2c_init(i2c0, 400000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    
    printf("Initializing LSM6DSOX...\n");
    // Initialize LSM6DSOX
    if (!lsm6dsox_init(i2c0)) {
        printf("Failed to initialize LSM6DSOX\n");
        return -1;
    }

    printf("Initializing Display...\n");
    // Initialize display
    display_init();
    
    // Variables
    lsm6dsox_data_t sensor_data;
    display_data_t display_data = {
        .pitch = 0,
        .roll = 0,
        .car_speed = -1,
        .movement_state = DISPLAY_STOPPED,
        .connected = false
    };
    
    absolute_time_t last_update = get_absolute_time();
    bool sensor_ok = false;

    printf("Starting main loop...\n");
    while (1) {
        if (absolute_time_diff_us(last_update, get_absolute_time()) >= UPDATE_RATE_MS * 1000) {
            // Read sensor data
            if (lsm6dsox_read_data(&sensor_data)) {
                sensor_ok = true;
                lsm6dsox_calculate_angles(&sensor_data);
                
                // Update display data
                display_data.pitch = sensor_data.pitch;
                display_data.roll = sensor_data.roll;
                
                // Determine movement state
                if (fabs(sensor_data.pitch) > fabs(sensor_data.roll)) {
                    if (sensor_data.pitch > 30)
                        display_data.movement_state = DISPLAY_FORWARD;
                    else if (sensor_data.pitch < -30)
                        display_data.movement_state = DISPLAY_BACKWARD;
                    else
                        display_data.movement_state = DISPLAY_STOPPED;
                } else {
                    if (sensor_data.roll > 30)
                        display_data.movement_state = DISPLAY_TURNING_RIGHT;
                    else if (sensor_data.roll < -30)
                        display_data.movement_state = DISPLAY_TURNING_LEFT;
                    else
                        display_data.movement_state = DISPLAY_STOPPED;
                }
                
                // Print debug info
                printf("Pitch: %.2f, Roll: %.2f\n", sensor_data.pitch, sensor_data.roll);
            } else {
                sensor_ok = false;
                printf("Sensor read failed!\n");
            }
            
            // Test mode: toggle connection status every 3 seconds
            static uint32_t toggle_time = 0;
            uint32_t current_time = to_ms_since_boot(get_absolute_time());
            if (current_time - toggle_time >= 3000) {
                display_data.connected = !display_data.connected;
                toggle_time = current_time;
            }
            
            // Update display
            display_update(&display_data);
            
            last_update = get_absolute_time();
        }
        
        sleep_ms(10);  // Yield to avoid tight loop
    }

    return 0;
}