#include "sensor_fusion.h"
#include <math.h>

#define COMP_FILTER_ALPHA 0.96f
#define DT 0.02f  // 50Hz update rate

static sensor_fusion_data_t last_data = {0};

void sensor_fusion_init(void) {
    last_data.pitch = 0;
    last_data.roll = 0;
    last_data.pitch_rate = 0;
    last_data.roll_rate = 0;
}

bool sensor_fusion_update(sensor_fusion_data_t* output, const lsm6dsox_data_t* raw_data) {
    // Calculate angles from accelerometer
    float accel_pitch = atan2f(raw_data->accel_x, 
        sqrtf(raw_data->accel_y * raw_data->accel_y + 
              raw_data->accel_z * raw_data->accel_z)) * 180.0f / M_PI;
              
    float accel_roll = atan2f(raw_data->accel_y, 
        raw_data->accel_z) * 180.0f / M_PI;
    
    // Apply complementary filter
    output->pitch = COMP_FILTER_ALPHA * (last_data.pitch + raw_data->gyro_x * DT) + 
                   (1.0f - COMP_FILTER_ALPHA) * accel_pitch;
                   
    output->roll = COMP_FILTER_ALPHA * (last_data.roll + raw_data->gyro_y * DT) + 
                  (1.0f - COMP_FILTER_ALPHA) * accel_roll;
    
    // Update rotation rates
    output->pitch_rate = raw_data->gyro_x;
    output->roll_rate = raw_data->gyro_y;
    
    // Store for next iteration
    last_data = *output;
    
    return true;
}