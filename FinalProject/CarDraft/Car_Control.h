#ifndef CAR_CONTROL_H
#define CAR_CONTROL_H

#include "drv8871.h"
#include "sensor_fusion.h"
#include "timer.h"

#define MAX_SPEED        255
#define TILT_THRESHOLD   5.0f
#define TURN_THRESHOLD   15.0f
#define SAFETY_TIMEOUT   500  // ms
#define SMOOTH_FACTOR    0.8f // For smooth transitions
#define UPDATE_INTERVAL  20   // ms

typedef struct {
    drv8871_dual_t* motors;
    uint8_t current_speed;
    uint8_t target_speed;
    uint8_t current_direction;
    bool emergency_stop;
    uint32_t last_command_time;
    uint32_t last_update_time;
    float last_speed;  // For smooth transitions
} car_state_t;

void car_init(car_state_t* state, drv8871_dual_t* motors);
void car_update(car_state_t* state, float tilt_angle, float turn_angle);
void car_emergency_stop(car_state_t* state);
bool car_check_safety(car_state_t* state);
bool car_should_update(car_state_t* state);

#endif // CAR_CONTROL_H