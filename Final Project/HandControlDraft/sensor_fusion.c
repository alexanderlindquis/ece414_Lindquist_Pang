#include "sensor_fusion.h"
#include <math.h>
#include "timer.h"

#define COMPLEMENTARY_FILTER_ALPHA 0.96f
#define MAX_SAFE_TILT_ANGLE 80.0f
#define MAX_SAFE_ROTATION_RATE 300.0f  // degrees per second

static float last_pitch = 0.0f;
static float last_roll = 0.0f;
static uint32_t last_update_time;

void sensor_fusion_init(void) {
    last_update_time = timer_read();
}

bool sensor_fusion_update(sensor_fusion_data_t* output, const lsm6dsox_data_t* raw_data) {
    uint32_t current_time = timer_read();
    float dt = timer_elapsed_us(last_update_time, current_time) / 1000000.0f;
    last_update_time = current_time;
    
    // Calculate angles from accelerometer
    float accel_pitch = atan2f(raw_data->acceleration_x, 
        sqrtf(raw_data->acceleration_y * raw_data->acceleration_y + 
              raw_data->acceleration_z * raw_data->acceleration_z)) * 180.0f / M_PI;
              
    float accel_roll = atan2f(raw_data->acceleration_y, 
        raw_data->acceleration_z) * 180.0f / M_PI;
    
    // Integrate gyroscope data
    float gyro_pitch = last_pitch + raw_data->gyro_x * dt;
    float gyro_roll = last_roll + raw_data->gyro_y * dt;
    
    // Complementary filter
    output->pitch = COMPLEMENTARY_FILTER_ALPHA * gyro_pitch + 
                   (1.0f - COMPLEMENTARY_FILTER_ALPHA) * accel_pitch;
    output->roll = COMPLEMENTARY_FILTER_ALPHA * gyro_roll + 
                  (1.0f - COMPLEMENTARY_FILTER_ALPHA) * accel_roll;
    
    // Calculate angular rates
    output->pitch_rate = raw_data->gyro_x;
    output->roll_rate = raw_data->gyro_y;
    
    // Store for next iteration
    last_pitch = output->pitch;
    last_roll = output->roll;
    
    // Check safety limits
    bool is_safe = true;
    
    // Check angle limits
    if (fabs(output->pitch) > MAX_SAFE_TILT_ANGLE || 
        fabs(output->roll) > MAX_SAFE_TILT_ANGLE) {
        output->error_flags |= SENSOR_ERROR_TILT_LIMIT;
        is_safe = false;
    }
    
    // Check rotation rate limits
    if (fabs(output->pitch_rate) > MAX_SAFE_ROTATION_RATE || 
        fabs(output->roll_rate) > MAX_SAFE_ROTATION_RATE) {
        output->error_flags |= SENSOR_ERROR_ROTATION_RATE;
        is_safe = false;
    }
    
    return is_safe;
}

float sensor_fusion_get_forward_tilt(const sensor_fusion_data_t* data) {
    return data->pitch;  // Forward/backward tilt
}

float sensor_fusion_get_turn_angle(const sensor_fusion_data_t* data) {
    return data->roll;   // Left/right tilt
}