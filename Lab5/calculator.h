#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    INIT,
    FIRST_NUM,
    OPERATOR,
    SECOND_NUM,
    RESULT,
    ERROR_DIV0,
    ERROR_OVERFLOW
} CalculatorState;

#define BTN_NONE    255
#define BTN_CLEAR   10
#define BTN_EQUAL   11
#define BTN_PLUS    12
#define BTN_MINUS   13
#define BTN_MULT    14
#define BTN_DIV     15

void calculator_init();
void tick_calculator(uint16_t button);

#endif 