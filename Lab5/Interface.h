#ifndef INTERFACE_H
#define INTERFACE_H

#include <stdint.h>
#include "pico/stdlib.h"
#include "TFTMaster.h"

#define BTN_NONE   255
#define BTN_CLEAR  10
#define BTN_EQUAL  11
#define BTN_PLUS   12
#define BTN_MINUS  13
#define BTN_MULT   14
#define BTN_DIV    15

void interface_init();
void drawInterface();
void display(const char* text, bool is_error);
uint16_t getButton(uint16_t x, uint16_t y);

#endif