// main.c
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "lsm6dsox.h"

// Define I2C pins
#define I2C_SDA 4
#define I2C_SCL 5

// Function to calculate magnitude
float calculate_magnitude(sensor_data_t *data) {
    return sqrtf(data->x * data->x + 
                 data->y * data->y + 
                 data->z * data->z);
}

// Function to detect motion
const char* detect_motion(float magnitude) {
    if (magnitude > 11.0f) return "MOVING!";
    if (magnitude < 9.5f) return "FALLING!";
    return "STABLE";
}

int main() {
    // Initialize USB serial
    stdio_init_all();
    
    // Wait for USB connection
    sleep_ms(3000);
    printf("\nLSM6DSOX Test Starting...\n");

    // Initialize I2C port
    i2c_init(i2c0, 400000);  // 400 kHz
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // Create and initialize sensor structure
    lsm6dsox_t sensor;
    if (!lsm6dsox_init(&sensor, i2c0, LSM6DSOX_I2CADDR_DEFAULT)) {
        printf("Error: Could not initialize LSM6DSOX.\n");
        while (1) { tight_loop_contents(); }
    }
    printf("LSM6DSOX initialized successfully!\n\n");

    // Variables for sensor readings
    sensor_data_t accel, gyro;
    float temperature;
    uint32_t reading_count = 0;
    absolute_time_t last_print = get_absolute_time();

    // Main loop
    while (1) {
        // Read sensor data
        if (lsm6dsox_read_accel(&sensor, &accel) && 
            lsm6dsox_read_gyro(&sensor, &gyro)) {
            
            temperature = lsm6dsox_read_temp(&sensor);
            float accel_mag = calculate_magnitude(&accel);
            const char* motion_status = detect_motion(accel_mag);

            // Print readings every 500ms
            if (absolute_time_diff_us(last_print, get_absolute_time()) >= 500000) {
                printf("\033[2J\033[H"); // Clear screen
                printf("Reading #%lu\n", ++reading_count);
                printf("====================\n\n");

                // Accelerometer data
                printf("Accelerometer (m/s^2):\n");
                printf("  X: %+7.2f\n", accel.x);
                printf("  Y: %+7.2f\n", accel.y);
                printf("  Z: %+7.2f\n", accel.z);
                printf("  Magnitude: %7.2f\n\n", accel_mag);

                // Gyroscope data
                printf("Gyroscope (rad/s):\n");
                printf("  X: %+7.2f\n", gyro.x);
                printf("  Y: %+7.2f\n", gyro.y);
                printf("  Z: %+7.2f\n\n", gyro.z);

                // Temperature and status
                printf("Temperature: %.1f°C\n", temperature);
                printf("Motion Status: %s\n", motion_status);

                last_print = get_absolute_time();
            }
        } else {
            printf("Error reading sensor data!\n");
            sleep_ms(1000);
        }

        sleep_ms(10); // Slight delay to prevent overwhelming the sensor
    }

    return 0;
}