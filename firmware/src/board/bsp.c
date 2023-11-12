/***************************************************************************************************
 * @file    : bsp.c
 * @brief   : Board Support Package source file
 * @author  : Janis Zaharans
 * @date    : Jun 8, 2022
 **************************************************************************************************/

/* > Includes *************************************************************************************/
#include "bsp.h"
#include "main.h"
#include "bsp_pins.h"
#include "button.h"

/* > Defines **************************************************************************************/


static void _initBuzzer(void);

static const io_t kBtnIo[BTN_COUNT] = {
  [RED]    = { .port = RED_BTN_PORT,     .pin = RED_BTN_PIN },
  [GREEN]  = { .port = GREEN_BTN_PORT,   .pin = GREEN_BTN_PIN },
  [YELLOW] = { .port = YELLOW_BTN_PORT,  .pin = YELLOW_BTN_PIN },
  [BLUE]   = { .port = BLUE_BTN_PORT,    .pin = BLUE_BTN_PIN },
};

static const io_t kLedIo[LED_COUNT] = {
  [RED]    = { .port = RED_LED_PORT,     .pin = RED_LED_PIN },
  [GREEN]  = { .port = GREEN_LED_PORT,   .pin = GREEN_LED_PIN },
  [YELLOW] = { .port = YELLOW_LED_PORT,  .pin = YELLOW_LED_PIN },
  [BLUE]   = { .port = BLUE_LED_PORT,    .pin = BLUE_LED_PIN },
};

/* > Local Variables ******************************************************************************/

static TIM_HandleTypeDef htim16;
static button_t s_btns[BTN_COUNT];

/* > Function Definitions *************************************************************************/

void Bsp_init(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  // GPIO Clock enable
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  // Initialize Buttons
  GPIO_InitStruct.Pin = RED_BTN_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(RED_BTN_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GREEN_BTN_PIN | YELLOW_BTN_PIN | BLUE_BTN_PIN;
  HAL_GPIO_Init(GREEN_BTN_PORT, &GPIO_InitStruct);

  // Initialize LEDs
  GPIO_InitStruct.Pin = LED_PIN_MASK;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_PORT, &GPIO_InitStruct);

  // Initialize Buzzer
  _initBuzzer();

  // Configure sleep
  __HAL_RCC_PWR_CLK_ENABLE();
  HAL_PWREx_EnablePullUpPullDownConfig();
  HAL_PWREx_EnableGPIOPullUp(PWR_GPIO_B, RED_BTN_PIN);
  HAL_PWREx_EnableGPIOPullUp(PWR_GPIO_A, GREEN_BTN_PIN);
  HAL_PWREx_EnableGPIOPullUp(PWR_GPIO_A, YELLOW_BTN_PIN);
  HAL_PWREx_EnableGPIOPullUp(PWR_GPIO_A, BLUE_BTN_PIN);
  HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN6_LOW);
  HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1_LOW);
  HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN4_LOW);
  HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN2_LOW);
}

void Bsp_deinit(void) {

}

void Bsp_sleep(void) {
  __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WUF6);
  __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WUF1);
  __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WUF4);
  __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WUF2);
  HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
}

void Bsp_standby(void) {
  __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WUF6);
  __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WUF1);
  __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WUF4);
  __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WUF2);
  HAL_PWR_EnterSTANDBYMode();
}

void Buzzer_beep(uint32_t freq) {
  uint32_t period = 1000000UL / freq;
  __HAL_TIM_SET_AUTORELOAD(&htim16, period);
  __HAL_TIM_SET_COMPARE(&htim16, TIM_CHANNEL_1, period / 2);
  HAL_TIM_PWM_Start(&htim16, TIM_CHANNEL_1);
}

void Buzzer_stop(void) {
  HAL_TIM_PWM_Stop(&htim16, TIM_CHANNEL_1);
}

void Led_setState(uint8_t led, bool state) {
  HAL_GPIO_WritePin(kLedIo[led].port, kLedIo[led].pin, state);
}

bool Key_isPress(uint8_t key) {
  return Button_isPress(&s_btns[key]);
}

bool Key_isRelease(uint8_t key) {
  return Button_isRelease(&s_btns[key]);
}

void Key_flush(void) {
  for(uint32_t i=0; i<BTN_COUNT; i++) {
    s_btns[i].press = 0;
    s_btns[i].release = 0;
  }
}

void Key_process(void) {
  uint32_t t = HAL_GetTick();
  for(uint32_t i=0; i<BTN_COUNT; i++)
    Button_process(&s_btns[i], (kBtnIo[i].port->IDR & kBtnIo[i].pin) == 0, t);
}


static void _initBuzzer(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  // Peripheral clock enable
  __HAL_RCC_TIM16_CLK_ENABLE();

  htim16.Instance = TIM16;
  htim16.Init.Prescaler = 64 - 1;
  htim16.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim16.Init.Period = 65535;
  htim16.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim16.Init.RepetitionCounter = 0;
  htim16.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim16) != HAL_OK)
  {
    Error_Handler();
  }

  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim16, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }


  GPIO_InitStruct.Pin = BUZZER_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF2_TIM16;
  HAL_GPIO_Init(BUZZER_PORT, &GPIO_InitStruct);
}