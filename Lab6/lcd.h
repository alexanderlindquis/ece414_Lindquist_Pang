#ifndef lcd_h
#define lcd_h

#include <stdio.h>         
#include <math.h>         
<<<<<<< HEAD
#include "pico/stdlib.h"      
#include "TFTMaster.h"

=======
#include "pico/stdlib.h"        
#include "TFTMaster.h"
#include "ic.h"
char buffer[10];
>>>>>>> 8a7e469995f4c22f99e1a86db6c5ac6c54816c2f


void lcd_init();

void dpcommand();

void dprpm();



#endif