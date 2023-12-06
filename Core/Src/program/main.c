#include "main.h"

#include "lcd.h"

#include <string.h>

void setup() {
  char *greeting = "Hello, world!";
  LCD_Init();
  LCD_Clear();
  LCD_DrawString(0, 0, greeting, strlen(greeting));
}

void loop() {
}
