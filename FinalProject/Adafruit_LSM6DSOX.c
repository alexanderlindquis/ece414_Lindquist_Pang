//LSM6DSOX Accelerometer Module which provides functions for initializing and converting accelerometer values to pitch and roll.
#include <stdio.h>
#include <math.h>
#include "hardware/i2c.h"
#include "pico/stdlib.h"

// LSM6DSOX Constants
#define LSM6DSOX_ADDRESS 0x6A
#define CTRL1_XL 0x10
#define OUTX_L_A 0x28
#define RANGE_2G 0.061  // Scaling factor for ±2g range in mg/LSB
#define ACCEL_SENSITIVITY (0.061 * 9.80665) // m/s² per LSB
#define PI 3.14159265359

// Function Prototypes
void lsm6dsox_init(i2c_inst_t *i2c);
void read_acceleration(i2c_inst_t *i2c, float *ax, float *ay, float *az);
void calculate_pitch_roll(float ax, float ay, float az, float *pitch, float *roll);

// Initialize LSM6DSOX
void lsm6dsox_init(i2c_inst_t *i2c) {
    uint8_t buffer[2];

    // Configure CTRL1_XL: Accelerometer 104Hz, ±2g
    buffer[0] = CTRL1_XL;
    buffer[1] = 0x10; // 104 Hz, ±2g
    i2c_write_blocking(i2c, LSM6DSOX_ADDRESS, buffer, 2, false);
}

// Read raw accelerometer data and convert to m/s²
void read_acceleration(i2c_inst_t *i2c, float *ax, float *ay, float *az) {
    uint8_t buffer[6];
    int16_t raw_x, raw_y, raw_z;

    // Read 6 bytes starting at OUTX_L_A
    uint8_t reg = OUTX_L_A;
    i2c_write_blocking(i2c, LSM6DSOX_ADDRESS, &reg, 1, true);
    i2c_read_blocking(i2c, LSM6DSOX_ADDRESS, buffer, 6, false);

    // Convert raw data to integers
    raw_x = (int16_t)((buffer[1] << 8) | buffer[0]);
    raw_y = (int16_t)((buffer[3] << 8) | buffer[2]);
    raw_z = (int16_t)((buffer[5] << 8) | buffer[4]);

    // Scale to m/s²
    *ax = raw_x * ACCEL_SENSITIVITY;
    *ay = raw_y * ACCEL_SENSITIVITY;
    *az = raw_z * ACCEL_SENSITIVITY;
}

// Calculate pitch and roll
void calculate_pitch_roll(float ax, float ay, float az, float *pitch, float *roll) {
    *pitch = atan2(-ax, sqrt(ay * ay + az * az)) * (180.0 / PI);
    *roll = atan2(ay, az) * (180.0 / PI);
}

// Main function for design verification
int main() {
    stdio_init_all(); // Initialize serial output
    i2c_init(i2c0, 400 * 1000); // 400 kHz I2C
    gpio_set_function(4, GPIO_FUNC_I2C);  // SDA
    gpio_set_function(5, GPIO_FUNC_I2C);  // SCL
    gpio_pull_up(4);
    gpio_pull_up(5);

    lsm6dsox_init(i2c0); // Initialize sensor

    float ax, ay, az;
    float pitch, roll;

    printf("LSM6DSOX Accelerometer Initialized. Reading Data...\n");

    while (1) {
        read_acceleration(i2c0, &ax, &ay, &az);
        calculate_pitch_roll(ax, ay, az, &pitch, &roll);
        printf("Ax: %.2f m/s², Ay: %.2f m/s², Az: %.2f m/s²\n", ax, ay, az);
        printf("Pitch: %.2f°, Roll: %.2f°\n", pitch, roll);
        sleep_ms(100); // Delay for readability
    }

    return 0;
}
