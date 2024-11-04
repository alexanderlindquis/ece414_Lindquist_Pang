#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "TouchScreen.h"
#include "TFTMaster.h"
#include "ts_lcd.h"
#include "ic.h"
float Kp, Ki, Kd;
float IntegMax = 200;
float IntegMin = 0;
float actualPrev, Integ = 0;
float desired;
float pwm_level;

static inline int32_t max (uint32_t a, uint32_t b) {
    if (a > b) return a;
    else return b;
}

static inline int32_t min (uint32_t a, uint32_t b) {
    if (a < b) return a;
    else return b;
}

float actuator_calc(){
    float actual = ic_getrpm();
    if(actual == NO_CAPTURE_READY){
        actual = actualPrev;
    }
    float Error = desired - actual;
    float Deriv = actual - actualPrev;
    actualPrev = actual;
    Integ = Integ + Error;
    if(Integ > IntegMax){
        Integ = IntegMax;
    }
    else if(Integ < IntegMin){
        Integ = IntegMin;
    }
    return Kp * Error + Ki * Integ - Kd * Deriv;
}

void controller_init(){
    pwm_level = 0x8000;
    pwm_pin_init(pwm_level);
}

void controller_tick(){
    uint16_t bias = (int)actuator_calc();
    if(bias < 0){
        pwm_level = max(pwm_level + bias, 0);
    }
    else if(bias > 0){
        pwm_level = min(pwm_level + bias, 0xffff);
    }
    pwm_pin_set_level(pwm_level);
}
