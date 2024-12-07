#ifndef SENSOR_FUSION_H
#define SENSOR_FUSION_H

#include "lsm6dsox.h"

typedef struct {
    float pitch;
    float roll;
    float pitch_rate;
    float roll_rate;
} sensor_fusion_data_t;

void sensor_fusion_init(void);
bool sensor_fusion_update(sensor_fusion_data_t* output, const lsm6dsox_data_t* raw_data);

#endif
