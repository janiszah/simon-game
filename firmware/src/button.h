/*
 * button.h
 *
 *  Created on: Mar 30, 2016
 *      Author: janis_zaharans
 */

#ifndef _BUTTON_H
#define _BUTTON_H

#include <stdint.h>
#include <stdbool.h>


#define BUTTON_LONG_PRESS_TIME  (500)

typedef struct
{
  uint16_t statePress;
  uint16_t stateRelease;

  uint8_t press;
  uint8_t release;
  uint8_t hold;

  uint8_t     longPressArm;
  uint8_t     longPress;

  uint32_t		pressTime;
  uint32_t		releaseTime;
  uint32_t		holdTime;
} button_t;


static inline void Button_process(button_t *btn, bool state, uint32_t time)
{
	btn->statePress = (btn->statePress<<1) | (state == 0) | 0xE000;
	if(btn->statePress == 0xF000) {
		btn->press = 1;
    btn->pressTime = time;
	}

	btn->stateRelease = (btn->stateRelease<<1) | (state != 0) | 0xE000;
	if(btn->stateRelease == 0xF000) {
		btn->release = 1;
    btn->releaseTime = time;
	}
}

static inline bool Button_isPress(button_t* btn) {
  bool state = btn->press;
  if(state)
    btn->press = 0;
  return state;
}

static inline bool Button_isRelease(button_t* btn) {
  bool state = btn->release;
  if(state)
    btn->release = 0;
  return state;
}


#endif /* BUTTON_H_ */
