#ifndef LSM6DSOX_H
#define LSM6DSOX_H

#include "pico/stdlib.h"
#include "hardware/i2c.h"

// Device-specific definitions
#define LSM6DSOX_CHIP_ID          0x6C
#define LSM6DSOX_I2C_ADDR         0x6A    // Default I2C address

// Registers
#define LSM6DSOX_FUNC_CFG_ACCESS  0x01
#define LSM6DSOX_PIN_CTRL         0x02
#define LSM6DSOX_INT1_CTRL        0x0D
#define LSM6DSOX_INT2_CTRL        0x0E
#define LSM6DSOX_WHOAMI           0x0F
#define LSM6DSOX_CTRL1_XL         0x10
#define LSM6DSOX_CTRL2_G          0x11
#define LSM6DSOX_CTRL3_C          0x12
#define LSM6DSOX_CTRL9_XL         0x18
#define LSM6DSOX_MASTER_CONFIG    0x14
#define LSM6DSOX_STATUS_REG       0x1E
#define LSM6DSOX_OUT_TEMP_L       0x20
#define LSM6DSOX_OUTX_L_G         0x22
#define LSM6DSOX_OUTX_L_A         0x28

// Data rate definitions
typedef enum {
    LSM6DSOX_RATE_SHUTDOWN,
    LSM6DSOX_RATE_12_5_HZ,
    LSM6DSOX_RATE_26_HZ,
    LSM6DSOX_RATE_52_HZ,
    LSM6DSOX_RATE_104_HZ,
    LSM6DSOX_RATE_208_HZ,
    LSM6DSOX_RATE_416_HZ,
    LSM6DSOX_RATE_833_HZ
} lsm6dsox_data_rate_t;

// Range definitions
typedef enum {
    LSM6DSOX_ACCEL_RANGE_2_G,
    LSM6DSOX_ACCEL_RANGE_16_G,
    LSM6DSOX_ACCEL_RANGE_4_G,
    LSM6DSOX_ACCEL_RANGE_8_G
} lsm6dsox_accel_range_t;

typedef enum {
    LSM6DSOX_GYRO_RANGE_250_DPS,
    LSM6DSOX_GYRO_RANGE_500_DPS,
    LSM6DSOX_GYRO_RANGE_1000_DPS,
    LSM6DSOX_GYRO_RANGE_2000_DPS
} lsm6dsox_gyro_range_t;

// Data structure
typedef struct {
    float acceleration_x;
    float acceleration_y;
    float acceleration_z;
    float gyro_x;
    float gyro_y;
    float gyro_z;
    float pitch;
    float roll;
    float temperature;
} lsm6dsox_data_t;

// Function prototypes
bool lsm6dsox_init(i2c_inst_t *i2c_port);
bool lsm6dsox_read_data(lsm6dsox_data_t *data);
void lsm6dsox_calculate_angles(lsm6dsox_data_t *data);
bool lsm6dsox_set_accel_data_rate(lsm6dsox_data_rate_t rate);
bool lsm6dsox_set_gyro_data_rate(lsm6dsox_data_rate_t rate);
bool lsm6dsox_set_accel_range(lsm6dsox_accel_range_t range);
bool lsm6dsox_set_gyro_range(lsm6dsox_gyro_range_t range);
bool lsm6dsox_data_ready(void);

#endif // LSM6DSOX_H