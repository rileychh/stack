#include "lcd.h"
#include "main.h"
#include "putchar.h"

#include <string.h>

void setup() {
  char *greeting = "Hello, world!";
  LCD_Init();
  LCD_Clear();
  LCD_DrawString(0, 0, greeting, strlen(greeting));
  printf("%s\r\n", greeting);
}

void loop() {
  HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
  extern UART_HandleTypeDef huart1;
  HAL_Delay(500);
}
