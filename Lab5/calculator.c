#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "calculator.h"
#include "interface.h"
#include "timer.h"

CalculatorState state = INIT;
int32_t val1 = 0;
int32_t val2 = 0;
char buffer[32];
char operator = 0;
bool second_num_started = false;

void calculator_init() {
    state = INIT;
    val1 = 0;
    val2 = 0;
    operator = 0;
    second_num_started = false;
    strcpy(buffer, "0");
    display(buffer, false);
}

bool check_overflow(int32_t a, int32_t b, char op) {
    if (op == '+') {
        if (b > 0 && a > INT_MAX - b) return true;
        if (b < 0 && a < INT_MIN - b) return true;
    }
    else if (op == '-') {
        if (b < 0 && a > INT_MAX + b) return true;
        if (b > 0 && a < INT_MIN + b) return true;
    }
    else if (op == '*') {
        if (a > 0 && b > 0 && a > INT_MAX / b) return true;
        if (a > 0 && b < 0 && b < INT_MIN / a) return true;
        if (a < 0 && b > 0 && a < INT_MIN / b) return true;
        if (a < 0 && b < 0 && a < INT_MAX / b) return true;
    }
    return false;
}

void perform_calculation(bool chain_operation) {
    int32_t result;

    // Check for division by zero
    if (operator == '/' && val2 == 0) {
        strcpy(buffer, "DIV0");
        state = ERROR_DIV0;
        display(buffer, true);
        sleep_ms(1000);
        calculator_init();
        return;
    }

    // Check for overflow before performing operation
    if (check_overflow(val1, val2, operator)) {
        strcpy(buffer, "ERROR");
        state = ERROR_OVERFLOW;
        display(buffer, true);
        sleep_ms(1000);
        calculator_init();
        return;
    }

    // Perform calculation
    switch(operator) {
        case '+':
        result = val1 + val2; 
        break;
        case '-': 
        result = val1 - val2; 
        break;
        case '*': 
        result = val1 * val2; 
        break;
        case '/': 
        result = val1 / val2; 
        break;
        default: return;
    }

    val1 = result;
    snprintf(buffer, sizeof(buffer), "%d", result);
    state = chain_operation ? OPERATOR : RESULT;
    display(buffer, false);
}


void tick_calculator(uint16_t button) {

    if (button == BTN_NONE) {
        return;
    }

    //Clear button
    if (button == BTN_CLEAR) {
        calculator_init();
        return;
    }

    // Check for overflow
    if (button <= 9) {
        if ((state == FIRST_NUM && (val1 > INT_MAX/10 || val1 < INT_MIN/10)) ||
            (state == SECOND_NUM && (val2 > INT_MAX/10 || val2 < INT_MIN/10))) {
            strcpy(buffer, "ERROR");
            display(buffer, true);
            sleep_ms(1000);
            calculator_init();
            return;
        }
    }

    // Process number input
    if (button <= 9) {
        switch(state) {
            case INIT:
                val1 = button;
                snprintf(buffer, sizeof(buffer), "%d", val1);
                state = FIRST_NUM;
                break;

            case FIRST_NUM:
                val1 = val1 * 10 + button;
                snprintf(buffer, sizeof(buffer), "%d", val1);
                break;

            case OPERATOR:
                state = SECOND_NUM;
                second_num_started = false; 
            
            case SECOND_NUM:
                if (!second_num_started) {
                    val2 = button;
                    second_num_started = true;
                } else {
                    val2 = val2 * 10 + button;
                }
                snprintf(buffer, sizeof(buffer), "%d%c%d", val1, operator, val2);
                break;

            case RESULT:
            case ERROR_DIV0:
            case ERROR_OVERFLOW:
                calculator_init();
                val1 = button;
                snprintf(buffer, sizeof(buffer), "%d", val1);
                state = FIRST_NUM;
                break;
        }
    }
    //operator
    else if (button >= BTN_PLUS && button <= BTN_DIV) {
        if (state == SECOND_NUM) {
            perform_calculation(true);
        }

        if (state == FIRST_NUM || state == RESULT || state == OPERATOR) {
            switch(button) {
                case BTN_PLUS:  
                operator = '+'; 
                break;
                case BTN_MINUS: 
                operator = '-'; 
                break;
                case BTN_MULT:  
                operator = '*'; 
                break;
                case BTN_DIV:   
                operator = '/'; 
                break;
            }
            snprintf(buffer, sizeof(buffer), "%d%c", val1, operator);
            state = OPERATOR;
            second_num_started = false;  // second_num_started for new input
            display(buffer, false);
        }
    }
    // Process equals button
    else if (button == BTN_EQUAL) {
        if (state == SECOND_NUM || state == RESULT) {
            perform_calculation(false);
        }
    }

    // Update display
    display(buffer, state == ERROR_DIV0 || state == ERROR_OVERFLOW);
}
