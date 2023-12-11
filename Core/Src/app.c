#include "lcd.h"
#include "main.h"
#include "uart_stdio.h"

#include <string.h>

extern UART_HandleTypeDef huart1;
extern ADC_HandleTypeDef hadc1;

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

  char s[100];
  printf("Enter a string: ");
  scanf("%s", s);
  printf("\r\nYou entered: %s\r\n", s);
}

void loop() {
  HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);

  HAL_ADC_Start(&hadc1);
  uint32_t adc_value = HAL_ADC_GetValue(&hadc1);
  HAL_ADC_Stop(&hadc1);
  printf("ADC value: %lu\r\n", adc_value);

  HAL_Delay(500);
}
