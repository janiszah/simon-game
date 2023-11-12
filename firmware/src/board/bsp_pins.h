#ifndef _BSP_PINS_H
#define _BSP_PINS_H


#define BUZZER_PORT   GPIOB
#define BUZZER_PIN    GPIO_PIN_8  // GPIO_PIN_7

// Buttons
#define RED_BTN_PORT      GPIOB
#define RED_BTN_PIN       GPIO_PIN_5  // WKUP6
#define GREEN_BTN_PORT    GPIOA
#define GREEN_BTN_PIN     GPIO_PIN_0  // WKUP1
#define YELLOW_BTN_PORT   GPIOA
#define YELLOW_BTN_PIN    GPIO_PIN_2  // WKUP4
#define BLUE_BTN_PORT     GPIOA
#define BLUE_BTN_PIN      GPIO_PIN_4  // WKUP2

// LEDs
#define RED_LED_PORT      GPIOA
#define RED_LED_PIN       GPIO_PIN_7
#define GREEN_LED_PORT    GPIOA
#define GREEN_LED_PIN     GPIO_PIN_1
#define YELLOW_LED_PORT   GPIOA
#define YELLOW_LED_PIN    GPIO_PIN_3
#define BLUE_LED_PORT     GPIOA
#define BLUE_LED_PIN      GPIO_PIN_5

#define LED_PORT          GPIOA
#define LED_PIN_MASK      (RED_LED_PIN | GREEN_LED_PIN | YELLOW_LED_PIN | BLUE_LED_PIN)


#endif // _BSP_PINS_H