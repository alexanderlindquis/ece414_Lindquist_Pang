// lsm6dsox.c
#include "lsm6dsox.h"
#include <math.h>
#include <string.h>

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
    sleep_ms(10);

    // Configure accelerometer: ±4g, 208 Hz, LPF enabled
    write_register(LSM6DSOX_CTRL1_XL, 
        (LSM6DSOX_RATE_208_HZ << 4) | 
        (LSM6DSOX_ACCEL_RANGE_4_G << 2) |
        0x01
    );

    // Configure gyroscope: 2000 dps, 208 Hz
    write_register(LSM6DSOX_CTRL2_G, 
        (LSM6DSOX_RATE_208_HZ << 4) | 
        (LSM6DSOX_GYRO_RANGE_2000_DPS << 2)
    );

    // Enable Block Data Update
    write_register(LSM6DSOX_CTRL3_C, 0x40);
    
    // Configure filters
    write_register(LSM6DSOX_CTRL6_C, 0x00);
    write_register(LSM6DSOX_CTRL8_XL, 0x09);

    // Enable data ready interrupts
    write_register(LSM6DSOX_INT1_CTRL, 0x03);

    return true;
}

bool lsm6dsox_read_data(lsm6dsox_raw_data_t *data) {
    uint8_t buffer[14];
    
    if (!read_multiple_registers(LSM6DSOX_OUT_TEMP_L, buffer, 14)) {
        return false;
    }

    // Process temperature
    int16_t temp_raw = (int16_t)(buffer[1] << 8 | buffer[0]);
    data->temperature = (temp_raw / 256.0f) + 25.0f;

    // Process gyroscope (raw to dps)
    float gyro_scale = 2000.0f / 32768.0f;  // For 2000 dps range
    int16_t gyro_x = (int16_t)(buffer[3] << 8 | buffer[2]);
    int16_t gyro_y = (int16_t)(buffer[5] << 8 | buffer[4]);
    int16_t gyro_z = (int16_t)(buffer[7] << 8 | buffer[6]);
    
    data->gyro_x = gyro_x * gyro_scale;
    data->gyro_y = gyro_y * gyro_scale;
    data->gyro_z = gyro_z * gyro_scale;

    // Process accelerometer (raw to g)
    float accel_scale = 4.0f / 32768.0f;  // For 4g range
    int16_t accel_x = (int16_t)(buffer[9] << 8 | buffer[8]);
    int16_t accel_y = (int16_t)(buffer[11] << 8 | buffer[10]);
    int16_t accel_z = (int16_t)(buffer[13] << 8 | buffer[12]);
    
    data->acceleration_x = accel_x * accel_scale;
    data->acceleration_y = accel_y * accel_scale;
    data->acceleration_z = accel_z * accel_scale;

    return true;
}

bool lsm6dsox_data_ready(void) {
    uint8_t status = read_register(LSM6DSOX_STATUS_REG);
    return (status & 0x03) == 0x03;
}