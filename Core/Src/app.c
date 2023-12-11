#include "app.h"

#include "lcd.h"
#include "main.h"
#include "uart_stdio.h"

#include <string.h>

extern UART_HandleTypeDef huart1;
extern ADC_HandleTypeDef hadc1;
extern TIM_HandleTypeDef htim6;

const char *GAME_BANNER =
  "\r\n"
  "                 _|_|_|    _|                          _|      \r\n"
  "               _|        _|_|_|_|    _|_|_|    _|_|_|  _|  _|  \r\n"
  "                 _|_|      _|      _|    _|  _|        _|_|    \r\n"
  "                     _|    _|      _|    _|  _|        _|  _|  \r\n"
  "               _|_|_|        _|_|    _|_|_|    _|_|_|  _|    _|\r\n"
  "                 Copyright © 2023 Stack for STM32 Developers   \r\n";

void setup() {
  char *greeting = "Hello, world!";
  LCD_Init();
  LCD_DrawString(0, 0, greeting, strlen(greeting));
  puts(GAME_BANNER);

  HAL_TIM_Base_Start_IT(&htim6);

  char s[100];
  printf("Enter a string: ");
  scanf("%s", s);
  printf("\r\nYou entered: %s\r\n", s);
}

void loop() {
  HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
  HAL_Delay(500);
}

/**
 * @brief TIM6 Update Interrupt Service Routine
 * This function runs every `htim6.Init.Period / 10` milliseconds.
 * @see MX_TIM6_Init()
 */
void on_tim6(void) {
  HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);

  HAL_ADC_Start(&hadc1);
  uint32_t adc_value = HAL_ADC_GetValue(&hadc1);
  HAL_ADC_Stop(&hadc1);
  printf("ADC value: %lu\r\n", adc_value);

  HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);
}
