#ifndef SENSOR_FUSION_H
#define SENSOR_FUSION_H

#include "pico/stdlib.h"
#include "lsm6dsox.h"

#define SENSOR_ERROR_NONE           0x00
#define SENSOR_ERROR_TILT_LIMIT     0x01
#define SENSOR_ERROR_ROTATION_RATE  0x02

typedef struct {
    float pitch;        // Forward/backward tilt angle
    float roll;         // Left/right tilt angle
    float pitch_rate;   // Rate of pitch change
    float roll_rate;    // Rate of roll change
    uint8_t error_flags;
} sensor_fusion_data_t;

void sensor_fusion_init(void);
bool sensor_fusion_update(sensor_fusion_data_t* output, const lsm6dsox_data_t* raw_data);
float sensor_fusion_get_forward_tilt(const sensor_fusion_data_t* data);
float sensor_fusion_get_turn_angle(const sensor_fusion_data_t* data);

#endif // SENSOR_FUSION_H