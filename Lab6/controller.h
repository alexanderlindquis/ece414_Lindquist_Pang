#include <stdio.h>
#include "pico/stdlib.h"
#include "ic.h"
#include "pwm_pin.h"

float Kp, Ki, Kd;
float IntegMax = 200;
float IntegMin = 0;
float actualPrev, Integ = 0;
float desired;
float pwm_level;

static inline int32_t max(uint32_t a, uint32_t b);
static inline int32_t min(uint32_t a, uint32_t b);
float actuator_calc();
void controller_init();
void controller_tick();