#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "TouchScreen.h"
#include "TFTMaster.h"
#include "ts_lcd.h"
#include "Interface.h"

int main()
{
    stdio_init_all();
    ts_lcd_init();
    drawInterface();
}