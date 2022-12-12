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

/* > Defines **************************************************************************************/


static void _initBuzzer(void);

/* > Local Variables ******************************************************************************/

static TIM_HandleTypeDef htim16;

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

}

void Bsp_deinit(void) {

}



static void _initBuzzer(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  // Peripheral clock enable
  __HAL_RCC_TIM16_CLK_ENABLE();

  htim16.Instance = TIM16;
  htim16.Init.Prescaler = 0;
  htim16.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim16.Init.Period = 65535;
  htim16.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim16.Init.RepetitionCounter = 0;
  htim16.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim16) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_TIM_OC_Init(&htim16) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_TIMING;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_OC_ConfigChannel(&htim16, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
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