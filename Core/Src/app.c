#include "lcd.h"
#include "main.h"
#include "uart_stdio.h"

#include <string.h>

extern UART_HandleTypeDef huart1;

const char *GAME_BANNER =
  CRLF
  "                 _|_|_|    _|                          _|      " CRLF
  "               _|        _|_|_|_|    _|_|_|    _|_|_|  _|  _|  " CRLF
  "                 _|_|      _|      _|    _|  _|        _|_|    " CRLF
  "                     _|    _|      _|    _|  _|        _|  _|  " CRLF
  "               _|_|_|        _|_|    _|_|_|    _|_|_|  _|    _|" CRLF
  "                 Copyright © 2023 Stack for STM32 Developers   " CRLF;

void setup() {
  char *greeting = "Hello, world!";
  LCD_Init();
  LCD_Clear();
  LCD_DrawString(0, 0, greeting, strlen(greeting));
  puts(GAME_BANNER);

  printf("Enter a character: ");
  char ch = '&';
  HAL_StatusTypeDef status = HAL_UART_Receive_IT(&huart1, (uint8_t *)&ch, 1);
  printf(
    CRLF
    "You entered: %c" CRLF
    "HAL_UART_Receive_IT status: %x" CRLF,
    ch,
    status
  );
}

void loop() {
  HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
  HAL_Delay(500);
}
