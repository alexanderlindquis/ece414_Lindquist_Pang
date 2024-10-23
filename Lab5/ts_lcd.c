// ts_lcd.c
/*this module is used to initialize all submodules and convert coordinates of touchscreen to coordinates of LCD*/
#include "ts_lcd.h"
#include "TouchScreen.h"
#include "TFTMaster.h"
#include "hardware/adc.h"
#include "pico/stdlib.h"

// LCD dimensions
#define LCD_WIDTH 319
#define LCD_HEIGHT 239
//These are the values from touchscreen_demo
#define TS_MINX 850
#define TS_MAXX 3300
#define TS_MINY 600
#define TS_MAXY 3600

#define MIN_PRESSURE 500
#define MAX_PRESSURE 50

void ts_lcd_init()
{
    adc_init();
    tft_init_hw();
    tft_begin();
    tft_setRotation(3);
    tft_fillScreen(ILI9340_BLACK);
}

bool get_ts_lcd(uint16_t *px, uint16_t *py)
{

    struct TSPoint p;
    p.x = 0;
    p.y = 0;
    p.z = 0;
    getPoint(&p);
    if (p.z < MIN_PRESSURE && p.z > MAX_PRESSURE)
    {
        *px = ts_lcd_xtransform(p.y);//since the x and y are swapped
        *py = ts_lcd_ytransform(p.x);
        return true;
    }
    else
    {
        return false;
    }
}

uint16_t ts_lcd_xtransform(uint16_t tx)
{
    uint16_t x = LCD_WIDTH - ((tx - TS_MINY) * LCD_WIDTH / (TS_MAXY - TS_MINY)); 
    if (x > LCD_WIDTH)
        x = LCD_WIDTH;
    return x;
}

uint16_t ts_lcd_ytransform(uint16_t ty)
{
    uint16_t y = (ty - TS_MINX) * LCD_HEIGHT / (TS_MAXX - TS_MINX);
    if (y > LCD_HEIGHT)
        y = LCD_HEIGHT;
    return y;
}
