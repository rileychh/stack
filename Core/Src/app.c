#include "app.h"

#include "lcd.h"
#include "main.h"
#include "uart_stdio.h"

#include <stdint.h>
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
  "                   Copyright (C) 2023 Arthur, Riley and Wei    \r\n";

void setup() {
  puts(GAME_BANNER);
  char s[100];
  printf("Enter a string: ");
  scanf("%s", s);
  printf("\r\nYou entered: %s\r\n", s);

  LCD_Init();

  HAL_TIM_Base_Start_IT(&htim6);
}

void loop() {
  HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
  HAL_Delay(500);
}

/**
 * @brief TIM6 Update Interrupt Service Routine
 *
 * This function runs every `htim6.Init.Period / 10` milliseconds.
 * @see MX_TIM6_Init()
 */
void on_tim6(void) {
  static int y_pos = 0;

  HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);

  LCD_Clear();
  char *line = "________";
  LCD_DrawString(1, y_pos, line, strlen(line));

  if (y_pos >= 40) {
    y_pos += 8;
  } else if (y_pos <= 0) {
    y_pos -= 8;
  }

  HAL_ADC_Start(&hadc1);
  uint32_t adc_value = HAL_ADC_GetValue(&hadc1);
  HAL_ADC_Stop(&hadc1);
  printf("ADC value: %lu\r\n", adc_value);

  HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);
}
