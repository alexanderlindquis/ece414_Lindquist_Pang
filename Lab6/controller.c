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