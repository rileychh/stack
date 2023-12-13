#include "app.h"

#include "lcd.h"
#include "main.h"
#include "uart_stdio.h"

#include <string.h>

const char *GAME_BANNER =
  "\r\n"
  "                 _|_|_|    _|                          _|      \r\n"
  "               _|        _|_|_|_|    _|_|_|    _|_|_|  _|  _|  \r\n"
  "                 _|_|      _|      _|    _|  _|        _|_|    \r\n"
  "                     _|    _|      _|    _|  _|        _|  _|  \r\n"
  "               _|_|_|        _|_|    _|_|_|    _|_|_|  _|    _|\r\n"
  "                   Copyright (C) 2023 Arthur, Riley and Wei    \r\n";

GameSession session;

void setup() {
  LCD_Init();

  session.score = 0;
  session.state = STATE_IDLE;

  HAL_TIM_Base_Start_IT(&htim6);
}

void loop() {
  switch (session.state) {
  case STATE_IDLE: handle_idle_state(); break;
  case STATE_PLAYING: handle_playing_state(); break;
  case STATE_PAUSED: handle_paused_state(); break;
  }
}

/**
 * @brief TIM6 Update Interrupt Service Routine
 *
 * This function runs every `htim6.Init.Period / 10` milliseconds.
 * @see MX_TIM6_Init()
 */
void on_tim6(void) {
  HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);
}

void handle_idle_state(void) {
  puts(GAME_BANNER);
  centered_puts("Press [Key] to start the game", CONSOLE_WIDTH);

  GPIO_PinState isPressed;
  do {
    isPressed = HAL_GPIO_ReadPin(User_Button_GPIO_Port, User_Button_Pin);
  } while (!isPressed);

  // TODO choose difficulty
  // TODO choose speed
}

void handle_playing_state(void) {
}

void handle_paused_state(void) {
}

uint32_t read_adc() {
  HAL_ADC_Start(&hadc1);
  uint32_t value = HAL_ADC_GetValue(&hadc1);
  HAL_ADC_Stop(&hadc1);
  return value;
}

int centered_puts(const char *s, uint8_t width) {
  size_t len = strlen(s);
  if (len > width) return puts(s);
  uint8_t padding = (width - len) >> 1; // half the value by shifting
  while (padding--) putchar(' ');
  return puts(s);
}
