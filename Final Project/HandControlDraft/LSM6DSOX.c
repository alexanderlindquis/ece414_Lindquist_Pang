#include "lsm6dsox.h"
#include <math.h>
#include <string.h>

// Static variables for internal use
static i2c_inst_t *i2c = NULL;
static lsm6dsox_accel_range_t current_accel_range = LSM6DSOX_ACCEL_RANGE_4_G;
static lsm6dsox_gyro_range_t current_gyro_range = LSM6DSOX_GYRO_RANGE_2000_DPS;

// Helper functions for I2C communication
static bool write_register(uint8_t reg, uint8_t value) {
    uint8_t buf[2] = {reg, value};
    return (i2c_write_blocking(i2c, LSM6DSOX_I2C_ADDR, buf, 2, false) == 2);
}

static uint8_t read_register(uint8_t reg) {
    uint8_t value;
    i2c_write_blocking(i2c, LSM6DSOX_I2C_ADDR, &reg, 1, true);
    i2c_read_blocking(i2c, LSM6DSOX_I2C_ADDR, &value, 1, false);
    return value;
}

static bool read_multiple_registers(uint8_t reg, uint8_t *buffer, uint8_t len) {
    i2c_write_blocking(i2c, LSM6DSOX_I2C_ADDR, &reg, 1, true);
    return (i2c_read_blocking(i2c, LSM6DSOX_I2C_ADDR, buffer, len, false) == len);
}

bool lsm6dsox_init(i2c_inst_t *i2c_port) {
    i2c = i2c_port;

    // Check device ID
    uint8_t whoami = read_register(LSM6DSOX_WHOAMI);
    if (whoami != LSM6DSOX_CHIP_ID) {
        return false;
    }

    // Reset device
    write_register(LSM6DSOX_CTRL3_C, 0x01);
    sleep_ms(10); // Wait for reset to complete

    // Configure default settings
    // Accelerometer: 104 Hz, 4g range
    write_register(LSM6DSOX_CTRL1_XL, 
        (LSM6DSOX_RATE_104_HZ << 4) | 
        (LSM6DSOX_ACCEL_RANGE_4_G << 2));

    // Gyroscope: 104 Hz, 2000 dps
    write_register(LSM6DSOX_CTRL2_G, 
        (LSM6DSOX_RATE_104_HZ << 4) | 
        (LSM6DSOX_GYRO_RANGE_2000_DPS << 2));

    // Enable Block Data Update
    write_register(LSM6DSOX_CTRL3_C, 0x40);
    
    // Disable I3C interface
    write_register(LSM6DSOX_CTRL9_XL, 0x02);

    return true;
}
bool lsm6dsox_read_data(lsm6dsox_data_t *data) {
    uint8_t buffer[14];  // For accel, gyro and temperature data
    
    // Read all data at once (temperature + gyro + accel)
    if (!read_multiple_registers(LSM6DSOX_OUT_TEMP_L, buffer, 14)) {
        return false;
    }

    // Process temperature data
    int16_t temp_raw = (int16_t)(buffer[1] << 8 | buffer[0]);
    data->temperature = (temp_raw / 256.0f) + 25.0f;

    // Process gyroscope data
    float gyro_scale;
    switch (current_gyro_range) {
        case LSM6DSOX_GYRO_RANGE_250_DPS:  gyro_scale = 250.0f; break;
        case LSM6DSOX_GYRO_RANGE_500_DPS:  gyro_scale = 500.0f; break;
        case LSM6DSOX_GYRO_RANGE_1000_DPS: gyro_scale = 1000.0f; break;
        case LSM6DSOX_GYRO_RANGE_2000_DPS: gyro_scale = 2000.0f; break;
        default: gyro_scale = 250.0f;
    }
    int16_t gyro_x = (int16_t)(buffer[3] << 8 | buffer[2]);
    int16_t gyro_y = (int16_t)(buffer[5] << 8 | buffer[4]);
    int16_t gyro_z = (int16_t)(buffer[7] << 8 | buffer[6]);
    
    data->gyro_x = (gyro_x * gyro_scale) / 32768.0f;
    data->gyro_y = (gyro_y * gyro_scale) / 32768.0f;
    data->gyro_z = (gyro_z * gyro_scale) / 32768.0f;

    // Process accelerometer data
    float accel_scale;
    switch (current_accel_range) {
        case LSM6DSOX_ACCEL_RANGE_2_G:  accel_scale = 2.0f; break;
        case LSM6DSOX_ACCEL_RANGE_4_G:  accel_scale = 4.0f; break;
        case LSM6DSOX_ACCEL_RANGE_8_G:  accel_scale = 8.0f; break;
        case LSM6DSOX_ACCEL_RANGE_16_G: accel_scale = 16.0f; break;
        default: accel_scale = 2.0f;
    }
    int16_t accel_x = (int16_t)(buffer[9] << 8 | buffer[8]);
    int16_t accel_y = (int16_t)(buffer[11] << 8 | buffer[10]);
    int16_t accel_z = (int16_t)(buffer[13] << 8 | buffer[12]);
    
    data->acceleration_x = (accel_x * accel_scale) / 32768.0f;
    data->acceleration_y = (accel_y * accel_scale) / 32768.0f;
    data->acceleration_z = (accel_z * accel_scale) / 32768.0f;

    return true;
}

void lsm6dsox_calculate_angles(lsm6dsox_data_t *data) {
    // Calculate pitch angle (rotation around X-axis)
    data->pitch = atan2f(data->acceleration_x, 
        sqrtf(data->acceleration_y * data->acceleration_y + 
              data->acceleration_z * data->acceleration_z)) * 180.0f / M_PI;
    
    // Calculate roll angle (rotation around Y-axis)
    data->roll = atan2f(data->acceleration_y, data->acceleration_z) * 180.0f / M_PI;
}

bool lsm6dsox_set_accel_data_rate(lsm6dsox_data_rate_t rate) {
    uint8_t current = read_register(LSM6DSOX_CTRL1_XL);
    current &= ~(0xF0);  // Clear rate bits
    current |= (rate << 4);
    return write_register(LSM6DSOX_CTRL1_XL, current);
}

bool lsm6dsox_set_gyro_data_rate(lsm6dsox_data_rate_t rate) {
    uint8_t current = read_register(LSM6DSOX_CTRL2_G);
    current &= ~(0xF0);  // Clear rate bits
    current |= (rate << 4);
    return write_register(LSM6DSOX_CTRL2_G, current);
}

bool lsm6dsox_set_accel_range(lsm6dsox_accel_range_t range) {
    uint8_t current = read_register(LSM6DSOX_CTRL1_XL);
    current &= ~(0x0C);  // Clear range bits
    current |= (range << 2);
    if (write_register(LSM6DSOX_CTRL1_XL, current)) {
        current_accel_range = range;
        return true;
    }
    return false;
}

bool lsm6dsox_set_gyro_range(lsm6dsox_gyro_range_t range) {
    uint8_t current = read_register(LSM6DSOX_CTRL2_G);
    current &= ~(0x0C);  // Clear range bits
    current |= (range << 2);
    if (write_register(LSM6DSOX_CTRL2_G, current)) {
        current_gyro_range = range;
        return true;
    }
    return false;
}

bool lsm6dsox_data_ready(void) {
    uint8_t status = read_register(LSM6DSOX_STATUS_REG);
    return (status & 0x03) == 0x03;  // Check if both accel and gyro data are ready
}