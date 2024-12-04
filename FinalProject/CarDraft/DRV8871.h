#ifndef DRV8871_H
#define DRV8871_H

#include "pico/stdlib.h"
#include "hardware/pwm.h"

#define DIRECTION_NONE     0
#define DIRECTION_FORWARD  1
#define DIRECTION_BACKWARD 2
#define TURN_LEFT         3
#define TURN_RIGHT        4

typedef struct {
    uint pin_in1;
    uint pin_in2;
    uint slice_num;
    uint chan_in1;
    uint chan_in2;
    uint8_t current_speed;
    uint8_t current_direction;
} drv8871_t;

typedef struct {
    drv8871_t* motor1;
    drv8871_t* motor2;
} drv8871_dual_t;

void drv8871_init(drv8871_t* motor, uint pin_in1, uint pin_in2);
void drv8871_set_speed(drv8871_t* motor, uint8_t speed, uint8_t direction);
void drv8871_dual_init(drv8871_dual_t* motors, 
                       uint m1_pin1, uint m1_pin2, 
                       uint m2_pin1, uint m2_pin2);
void drv8871_drive(drv8871_dual_t* motors, uint8_t speed, uint8_t direction);
void drv8871_turn(drv8871_dual_t* motors, uint8_t speed, uint8_t direction);
void drv8871_stop(drv8871_dual_t* motors);

#endif // DRV8871_H