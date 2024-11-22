// lsm6dsox.h
#ifndef PICO_LSM6DSOX_H
#define PICO_LSM6DSOX_H

#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

// LSM6DSOX Constants
#define LSM6DSOX_CHIP_ID             0x6C
#define LSM6DSOX_I2CADDR_DEFAULT     0x6A

// Register addresses
#define LSM6DSOX_FUNC_CFG_ACCESS     0x01
#define LSM6DSOX_PIN_CTRL            0x02
#define LSM6DSOX_FIFO_CTRL1          0x07
#define LSM6DSOX_FIFO_CTRL2          0x08
#define LSM6DSOX_FIFO_CTRL3          0x09
#define LSM6DSOX_FIFO_CTRL4          0x0A
#define LSM6DSOX_INT1_CTRL           0x0D
#define LSM6DSOX_INT2_CTRL           0x0E
#define LSM6DSOX_WHOAMI              0x0F
#define LSM6DSOX_CTRL1_XL            0x10
#define LSM6DSOX_CTRL2_G             0x11
#define LSM6DSOX_CTRL3_C             0x12
#define LSM6DSOX_CTRL4_C             0x13
#define LSM6DSOX_CTRL5_C             0x14
#define LSM6DSOX_CTRL6_C             0x15
#define LSM6DSOX_CTRL7_G             0x16
#define LSM6DSOX_CTRL8_XL            0x17
#define LSM6DSOX_CTRL9_XL            0x18
#define LSM6DSOX_CTRL10_C            0x19
#define LSM6DSOX_OUT_TEMP_L          0x20
#define LSM6DSOX_OUT_TEMP_H          0x21
#define LSM6DSOX_OUTX_L_G            0x22
#define LSM6DSOX_OUTX_L_A            0x28
#define LSM6DSOX_MASTER_CONFIG       0x14

// Conversion constants
#define SENSORS_GRAVITY_STANDARD     (9.80665f)
#define SENSORS_DPS_TO_RADS         (0.017453293f)

// Type definitions
typedef enum {
    LSM6DSOX_RATE_SHUTDOWN,
    LSM6DSOX_RATE_12_5_HZ,
    LSM6DSOX_RATE_26_HZ,
    LSM6DSOX_RATE_52_HZ,
    LSM6DSOX_RATE_104_HZ,
    LSM6DSOX_RATE_208_HZ,
    LSM6DSOX_RATE_416_HZ,
    LSM6DSOX_RATE_833_HZ,
    LSM6DSOX_RATE_1_66K_HZ,
    LSM6DSOX_RATE_3_33K_HZ,
    LSM6DSOX_RATE_6_66K_HZ
} lsm6dsox_data_rate_t;

typedef enum {
    LSM6DSOX_ACCEL_RANGE_2_G,
    LSM6DSOX_ACCEL_RANGE_16_G,
    LSM6DSOX_ACCEL_RANGE_4_G,
    LSM6DSOX_ACCEL_RANGE_8_G
} lsm6dsox_accel_range_t;

typedef enum {
    LSM6DSOX_GYRO_RANGE_125_DPS  = 0x02,
    LSM6DSOX_GYRO_RANGE_250_DPS  = 0x00,
    LSM6DSOX_GYRO_RANGE_500_DPS  = 0x04,
    LSM6DSOX_GYRO_RANGE_1000_DPS = 0x08,
    LSM6DSOX_GYRO_RANGE_2000_DPS = 0x0C
} lsm6dsox_gyro_range_t;

typedef struct {
    i2c_inst_t* i2c;
    uint8_t addr;
    lsm6dsox_accel_range_t accel_range;
    lsm6dsox_gyro_range_t gyro_range;
    float temperature_sensitivity;
} lsm6dsox_t;

typedef struct {
    float x;
    float y;
    float z;
} sensor_data_t;

// Function prototypes
bool lsm6dsox_init(lsm6dsox_t* dev, i2c_inst_t* i2c, uint8_t addr);
void lsm6dsox_reset(lsm6dsox_t* dev);
bool lsm6dsox_read_accel(lsm6dsox_t* dev, sensor_data_t* accel);
bool lsm6dsox_read_gyro(lsm6dsox_t* dev, sensor_data_t* gyro);
float lsm6dsox_read_temp(lsm6dsox_t* dev);
void lsm6dsox_set_accel_range(lsm6dsox_t* dev, lsm6dsox_accel_range_t range);
void lsm6dsox_set_gyro_range(lsm6dsox_t* dev, lsm6dsox_gyro_range_t range);
void lsm6dsox_set_accel_data_rate(lsm6dsox_t* dev, lsm6dsox_data_rate_t rate);
void lsm6dsox_set_gyro_data_rate(lsm6dsox_t* dev, lsm6dsox_data_rate_t rate);

#endif // PICO_LSM6DSOX_H