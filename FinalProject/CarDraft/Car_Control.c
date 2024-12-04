#include "car_control.h"
#include <math.h>

void car_init(car_state_t* state, drv8871_dual_t* motors) {
    state->motors = motors;
    state->current_speed = 0;
    state->target_speed = 0;
    state->current_direction = DIRECTION_NONE;
    state->emergency_stop = false;
    state->last_command_time = timer_read();
    state->last_update_time = timer_read();
    state->last_speed = 0;
}

bool car_should_update(car_state_t* state) {
    uint32_t current_time = timer_read();
    if (timer_elapsed_ms(state->last_update_time, current_time) >= UPDATE_INTERVAL) {
        state->last_update_time = current_time;
        return true;
    }
    return false;
}

void car_update(car_state_t* state, float tilt_angle, float turn_angle) {
    if (state->emergency_stop) {
        return;
    }
    
    state->last_command_time = timer_read();
    
    // Only update at fixed intervals for smooth control
    if (!car_should_update(state)) {
        return;
    }
    
    // Calculate speed from tilt with smooth transition
    uint8_t target_speed = 0;
    if (fabs(tilt_angle) > TILT_THRESHOLD) {
        float speed_factor = fabs(tilt_angle) / 90.0f;
        if (speed_factor > 1.0f) speed_factor = 1.0f;
        
        // Apply half speed at 45±10 degrees
        if (fabs(tilt_angle - 45.0f) <= 10.0f) {
            speed_factor *= 0.5f;
        }
        
        target_speed = (uint8_t)(speed_factor * MAX_SPEED);
    }
    
    // Smooth speed transitions
    float smooth_speed = state->last_speed * SMOOTH_FACTOR + 
                        target_speed * (1.0f - SMOOTH_FACTOR);
    uint8_t speed = (uint8_t)smooth_speed;
    state->last_speed = smooth_speed;
    
    // Handle turning
    if (fabs(turn_angle) > TURN_THRESHOLD) {
        if (speed == 0) {
            // Turn in place
            speed = (uint8_t)((fabs(turn_angle) / 90.0f) * MAX_SPEED);
            drv8871_turn(state->motors, speed, 
                turn_angle > 0 ? TURN_RIGHT : TURN_LEFT);
        } else {
            // Proportional turning while moving
            float turn_factor = fabs(turn_angle) / 90.0f;
            if (turn_factor > 1.0f) turn_factor = 1.0f;
            
            // Sharp turn at 60 degrees
            if (fabs(turn_angle) >= 60.0f) {
                turn_factor = 1.0f;
            }
            
            uint8_t inner_speed = (uint8_t)(speed * (1.0f - turn_factor));
            uint8_t direction = tilt_angle > 0 ? DIRECTION_FORWARD : DIRECTION_BACKWARD;
            
            if (turn_angle > 0) {
                drv8871_set_speed(state->motors->motor1, inner_speed, direction);
                drv8871_set_speed(state->motors->motor2, speed, direction);
            } else {
                drv8871_set_speed(state->motors->motor1, speed, direction);
                drv8871_set_speed(state->motors->motor2, inner_speed, direction);
            }
        }
    } else {
        // Straight movement
        uint8_t direction = tilt_angle > 0 ? DIRECTION_FORWARD : DIRECTION_BACKWARD;
        drv8871_drive(state->motors, speed, direction);
    }
    
    state->current_speed = speed;
}

void car_emergency_stop(car_state_t* state) {
    state->emergency_stop = true;
    drv8871_stop(state->motors);
    state->current_speed = 0;
    state->current_direction = DIRECTION_NONE;
    state->last_speed = 0;
}

bool car_check_safety(car_state_t* state) {
    uint32_t current_time = timer_read();
    if (timer_elapsed_ms(state->last_command_time, current_time) > SAFETY_TIMEOUT) {
        car_emergency_stop(state);
        return false;
    }
    return true;
}