#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "lsm6dsox.h"
#include "hc05.h"
#include "display.h"
#include "protocol.h"

#define I2C_SDA 4
#define I2C_SCL 5
#define UPDATE_RATE_MS 20

// Replace with your car's HC-05 address
#define CAR_BT_ADDRESS "98:D3:31:FC:59:F0"

int main() {
    stdio_init_all();
    
    // Initialize I2C
    i2c_init(i2c0, 400000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    lsm6dsox_t imu;
    if (!lsm6dsox_init(&imu, i2c0, LSM6DSOX_I2CADDR_DEFAULT)) {
        printf("Failed to initialize LSM6DSOX\n");
        return -1;
    }

    // Initialize HC-05 as master
    hc05_init(HC05_MASTER_MODE, CAR_BT_ADDRESS);

    // Initialize display
    display_init();
    display_data_t display_data = {0};

    char bt_message[64];
    absolute_time_t last_update = get_absolute_time();

    while (1) {
        if (absolute_time_diff_us(last_update, get_absolute_time()) >= UPDATE_RATE_MS * 1000) {
            sensor_data_t accel_data;
            
            // Read accelerometer
            if (lsm6dsox_read_accel(&imu, &accel_data)) {
                // Calculate angles
                float tilt_angle = atan2f(accel_data.x, 
                    sqrtf(accel_data.y * accel_data.y + accel_data.z * accel_data.z)) 
                    * 180.0f / M_PI;
                
                float turn_angle = atan2f(accel_data.y, accel_data.z) 
                    * 180.0f / M_PI;

                // Send control message
                snprintf(bt_message, sizeof(bt_message), 
                        "%c,%.2f,%.2f\n", 
                        MSG_CONTROL, tilt_angle, turn_angle);
                hc05_send(bt_message, strlen(bt_message));

                // Update display
                display_data.current_speed = abs((int)(tilt_angle * 2.55f));
                if (abs(turn_angle) > 30.0f) {
                    display_data.state = turn_angle > 0 ? 
                        DISPLAY_TURNING_RIGHT : DISPLAY_TURNING_LEFT;
                } else {
                    display_data.state = tilt_angle > 0 ? 
                        DISPLAY_FORWARD : DISPLAY_BACKWARD;
                }
            }

            // Receive car status
            if (hc05_receive(bt_message, sizeof(bt_message))) {
                int speed, battery;
                if (sscanf(bt_message, "S,%d,%d", &speed, &battery) == 2) {
                    display_data.battery_level = battery;
                } else if (bt_message[0] == MSG_ERROR) {
                    display_data.emergency_stopped = true;
                }
            }

            // Update connection status
            hc05_status_t bt_status = hc05_get_status();
            display_data.connection_status = (bt_status.state == HC05_STATE_CONNECTED);
            
            // Update display
            display_update(&display_data);
            
            last_update = get_absolute_time();
        }
        
        sleep_ms(1);  // Yield to avoid tight loop
    }

    return 0;
}