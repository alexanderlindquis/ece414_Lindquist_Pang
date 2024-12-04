#include "drv8871.h"

#define PWM_WRAP 255

static void init_pwm_pin(uint pin, uint* slice_num, uint* channel) {
    gpio_set_function(pin, GPIO_FUNC_PWM);
    *slice_num = pwm_gpio_to_slice_num(pin);
    *channel = pwm_gpio_to_channel(pin);
    
    pwm_set_wrap(*slice_num, PWM_WRAP);
    pwm_set_enabled(*slice_num, true);
}

void drv8871_init(drv8871_t* motor, uint pin_in1, uint pin_in2) {
    motor->pin_in1 = pin_in1;
    motor->pin_in2 = pin_in2;
    motor->current_speed = 0;
    motor->current_direction = DIRECTION_NONE;

    // Initialize PWM for both pins
    init_pwm_pin(pin_in1, &motor->slice_num, &motor->chan_in1);
    init_pwm_pin(pin_in2, &motor->slice_num, &motor->chan_in2);
}

void drv8871_set_speed(drv8871_t* motor, uint8_t speed, uint8_t direction) {
    switch (direction) {
        case DIRECTION_FORWARD:
            pwm_set_chan_level(motor->slice_num, motor->chan_in1, speed);
            pwm_set_chan_level(motor->slice_num, motor->chan_in2, 0);
            break;
            
        case DIRECTION_BACKWARD:
            pwm_set_chan_level(motor->slice_num, motor->chan_in1, 0);
            pwm_set_chan_level(motor->slice_num, motor->chan_in2, speed);
            break;
            
        default:
            pwm_set_chan_level(motor->slice_num, motor->chan_in1, 0);
            pwm_set_chan_level(motor->slice_num, motor->chan_in2, 0);
            break;
    }
    
    motor->current_speed = speed;
    motor->current_direction = direction;
}

void drv8871_dual_init(drv8871_dual_t* motors, uint m1_pin1, uint m1_pin2, uint m2_pin1, uint m2_pin2) {
    motors->motor1 = malloc(sizeof(drv8871_t));
    motors->motor2 = malloc(sizeof(drv8871_t));
    
    drv8871_init(motors->motor1, m1_pin1, m1_pin2);
    drv8871_init(motors->motor2, m2_pin1, m2_pin2);
}

void drv8871_drive(drv8871_dual_t* motors, uint8_t speed, uint8_t direction) {
    drv8871_set_speed(motors->motor1, speed, direction);
    drv8871_set_speed(motors->motor2, speed, direction);
}

void drv8871_turn(drv8871_dual_t* motors, uint8_t speed, uint8_t direction) {
    switch (direction) {
        case TURN_LEFT:
            drv8871_set_speed(motors->motor1, speed, DIRECTION_BACKWARD);
            drv8871_set_speed(motors->motor2, speed, DIRECTION_FORWARD);
            break;
            
        case TURN_RIGHT:
            drv8871_set_speed(motors->motor1, speed, DIRECTION_FORWARD);
            drv8871_set_speed(motors->motor2, speed, DIRECTION_BACKWARD);
            break;
    }
}

void drv8871_stop(drv8871_dual_t* motors) {
    drv8871_set_speed(motors->motor1, 0, DIRECTION_NONE);
    drv8871_set_speed(motors->motor2, 0, DIRECTION_NONE);
}