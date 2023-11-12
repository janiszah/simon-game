#ifndef _BSP_H
#define _BSP_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32g0xx_hal.h"


enum btn_e {
  RED = 0,
  GREEN,
  YELLOW,
  BLUE,
};

#define BTN_COUNT (4)
#define LED_COUNT (4)

typedef struct {
  GPIO_TypeDef* port;
  uint32_t pin;
} io_t;


void Bsp_init(void);
void Bsp_deinit(void);
void Bsp_sleep(void);
void Bsp_standby(void);

void Buzzer_beep(uint32_t freq);
void Buzzer_stop(void);

void Led_setState(uint8_t led, bool state);

bool Key_isPress(uint8_t key);
bool Key_isRelease(uint8_t key);
void Key_flush(void);
void Key_process(void);

#endif // _BSP_H