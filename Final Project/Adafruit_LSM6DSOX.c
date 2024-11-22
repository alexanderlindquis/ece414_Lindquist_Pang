#include "lsm6dsox.h"

// Private function prototypes
static uint8_t read_reg(lsm6dsox_t* dev, uint8_t reg);
static void write_reg(lsm6dsox_t* dev, uint8_t reg, uint8_t value);
static bool read_regs(lsm6dsox_t* dev, uint8_t reg, uint8_t* buffer, uint8_t len);

bool lsm6dsox_init(lsm6dsox_t* dev, i2c_inst_t* i2c, uint8_t addr) {
    dev->i2c = i2c;
    dev->addr = addr;
    dev->temperature_sensitivity = 256.0f;

    // Check device ID
    uint8_t chip_id = read_reg(dev, LSM6DSOX_WHOAMI);
    if (chip_id != LSM6DSOX_CHIP_ID) {
        printf("Wrong chip ID! Expected 0x%02X, got 0x%02X\n", 
               LSM6DSOX_CHIP_ID, chip_id);
        return false;
    }

    // Reset device
    lsm6dsox_reset(dev);
    sleep_ms(10);

    // Enable Block Data Update
    write_reg(dev, LSM6DSOX_CTRL3_C, 0x40);
    
    // Disable I3C
    uint8_t ctrl9 = read_reg(dev, LSM6DSOX_CTRL9_XL);
    ctrl9 |= (1 << 1);
    write_reg(dev, LSM6DSOX_CTRL9_XL, ctrl9);

    // Configure default settings
    lsm6dsox_set_accel_data_rate(dev, LSM6DSOX_RATE_104_HZ);
    lsm6dsox_set_accel_range(dev, LSM6DSOX_ACCEL_RANGE_4_G);
    lsm6dsox_set_gyro_data_rate(dev, LSM6DSOX_RATE_104_HZ);
    lsm6dsox_set_gyro_range(dev, LSM6DSOX_GYRO_RANGE_2000_DPS);

    return true;
}

void lsm6dsox_reset(lsm6dsox_t* dev) {
    write_reg(dev, LSM6DSOX_CTRL3_C, 0x01);
    sleep_ms(10);
}

bool lsm6dsox_read_accel(lsm6dsox_t* dev, sensor_data_t* accel) {
    uint8_t buffer[6];
    if (!read_regs(dev, LSM6DSOX_OUTX_L_A, buffer, 6)) {
        return false;
    }

    int16_t raw_x = (buffer[1] << 8) | buffer[0];
    int16_t raw_y = (buffer[3] << 8) | buffer[2];
    int16_t raw_z = (buffer[5] << 8) | buffer[4];

    float accel_scale;
    switch (dev->accel_range) {
        case LSM6DSOX_ACCEL_RANGE_16_G: accel_scale = 0.488f; break;
        case LSM6DSOX_ACCEL_RANGE_8_G:  accel_scale = 0.244f; break;
        case LSM6DSOX_ACCEL_RANGE_4_G:  accel_scale = 0.122f; break;
        case LSM6DSOX_ACCEL_RANGE_2_G:  accel_scale = 0.061f; break;
        default: accel_scale = 0.122f; break;
    }

    accel->x = raw_x * accel_scale * SENSORS_GRAVITY_STANDARD / 1000.0f;
    accel->y = raw_y * accel_scale * SENSORS_GRAVITY_STANDARD / 1000.0f;
    accel->z = raw_z * accel_scale * SENSORS_GRAVITY_STANDARD / 1000.0f;

    return true;
}

bool lsm6dsox_read_gyro(lsm6dsox_t* dev, sensor_data_t* gyro) {
    uint8_t buffer[6];
    if (!read_regs(dev, LSM6DSOX_OUTX_L_G, buffer, 6)) {
        return false;
    }

    int16_t raw_x = (buffer[1] << 8) | buffer[0];
    int16_t raw_y = (buffer[3] << 8) | buffer[2];
    int16_t raw_z = (buffer[5] << 8) | buffer[4];

    float gyro_scale;
    switch (dev->gyro_range) {
        case LSM6DSOX_GYRO_RANGE_2000_DPS: gyro_scale = 70.0f;  break;
        case LSM6DSOX_GYRO_RANGE_1000_DPS: gyro_scale = 35.0f;  break;
        case LSM6DSOX_GYRO_RANGE_500_DPS:  gyro_scale = 17.50f; break;
        case LSM6DSOX_GYRO_RANGE_250_DPS:  gyro_scale = 8.75f;  break;
        case LSM6DSOX_GYRO_RANGE_125_DPS:  gyro_scale = 4.375f; break;
        default: gyro_scale = 70.0f; break;
    }

    gyro->x = raw_x * gyro_scale * SENSORS_DPS_TO_RADS / 1000.0f;
    gyro->y = raw_y * gyro_scale * SENSORS_DPS_TO_RADS / 1000.0f;
    gyro->z = raw_z * gyro_scale * SENSORS_DPS_TO_RADS / 1000.0f;

    return true;
}

float lsm6dsox_read_temp(lsm6dsox_t* dev) {
    uint8_t buffer[2];
    if (!read_regs(dev, LSM6DSOX_OUT_TEMP_L, buffer, 2)) {
        return 0.0f;
    }
    
    int16_t raw_temp = (buffer[1] << 8) | buffer[0];
    return (raw_temp / dev->temperature_sensitivity) + 25.0f;
}

void lsm6dsox_set_accel_range(lsm6dsox_t* dev, lsm6dsox_accel_range_t range) {
    uint8_t reg = read_reg(dev, LSM6DSOX_CTRL1_XL);
    reg &= ~(0b11 << 2);
    reg |= (range << 2);
    write_reg(dev, LSM6DSOX_CTRL1_XL, reg);
    dev->accel_range = range;
}

void lsm6dsox_set_gyro_range(lsm6dsox_t* dev, lsm6dsox_gyro_range_t range) {
    uint8_t reg = read_reg(dev, LSM6DSOX_CTRL2_G);
    reg &= ~(0b1111);
    reg |= range;
    write_reg(dev, LSM6DSOX_CTRL2_G, reg);
    dev->gyro_range = range;
}

void lsm6dsox_set_accel_data_rate(lsm6dsox_t* dev, lsm6dsox_data_rate_t rate) {
    uint8_t reg = read_reg(dev, LSM6DSOX_CTRL1_XL);
    reg &= ~(0b1111 << 4);
    reg |= (rate << 4);
    write_reg(dev, LSM6DSOX_CTRL1_XL, reg);
}

void lsm6dsox_set_gyro_data_rate(lsm6dsox_t* dev, lsm6dsox_data_rate_t rate) {
    uint8_t reg = read_reg(dev, LSM6DSOX_CTRL2_G);
    reg &= ~(0b1111 << 4);
    reg |= (rate << 4);
    write_reg(dev, LSM6DSOX_CTRL2_G, reg);
}

// Private I2C communication functions
static uint8_t read_reg(lsm6dsox_t* dev, uint8_t reg) {
    uint8_t value;
    i2c_write_blocking(dev->i2c, dev->addr, &reg, 1, true);
    i2c_read_blocking(dev->i2c, dev->addr, &value, 1, false);
    return value;
}

static void write_reg(lsm6dsox_t* dev, uint8_t reg, uint8_t value) {
    uint8_t buffer[2] = {reg, value};
    i2c_write_blocking(dev->i2c, dev->addr, buffer, 2, false);
}

static bool read_regs(lsm6dsox_t* dev, uint8_t reg, uint8_t* buffer, uint8_t len) {
    i2c_write_blocking(dev->i2c, dev->addr, &reg, 1, true);
    return (i2c_read_blocking(dev->i2c, dev->addr, buffer, len, false) == len);
}