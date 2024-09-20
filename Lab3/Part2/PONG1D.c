// Main module for PONG1D Game Logic

//Include necessary modules/libraries
#include "sw_in.h"

static enum PONG_States (LEFT, RIGHT, SERVE, BLINK} PONG_State;
//main logic
int main(){
  tBtn = timer_read();
  tPong = timer_read();
  uint32 PONGPD_MS = 300;
  while(true){
    tCurrent = timer_read();
    if(timer_elapsed_ms(tBtn, tCurrent) >= DEBOUNCEPD_MS){
      debounce_sw1_tick();
      deounce_sw2_tick();
    }
    if(debounce_sw1_pressed() || timer_elapsed_ms(tPong, tCurrent) >= PONGPD_MS){
      pong_tick();
    }
    
  }
}

//Update Game State based on button presses, current active player, 
void pong_tick(){
  switch(PONG_State){
    case SERVE:
      if(P0 && debounce_sw1_pressed()){
        PONG_State = RIGHT;
        P0 = false;
      }
      else if(!P0 && debounce_sw2_pressed()){
        PONG_State = RIGHT;
        P0 = true;
      }
      break;
    case LEFT:
      
      break;
    case RIGHT:
      
      break;
    case BLINK:
      
      break;
  }
}
