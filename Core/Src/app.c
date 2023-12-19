#include "app.h"

#include "lcd.h"
#include "main.h"
#include "uart_stdio.h"

#include <string.h>
#include <stdlib.h>

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
  session.state = STATE_INIT;

  HAL_TIM_Base_Start_IT(&htim6);
}

void loop() {
  switch (session.state) {
  case STATE_INIT: handle_init_state(); break;
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

void handle_init_state(void) {
  puts(GAME_BANNER);
  centered_puts("Press [Key] to start the game", CONSOLE_WIDTH);

  // LCD
  LCD_DrawString(6, 45, "Start", 5); // 假设 "Start" 在第100行显示

  GPIO_PinState isPressed;
  do {
    isPressed = !HAL_GPIO_ReadPin(User_Button_GPIO_Port, User_Button_Pin);
  } while (!isPressed);

  session.state = STATE_PLAYING;
}

void brick_spawning(void) {
  printf("brick spawned\r\n");
}

int brick_place(void) {
  int flag = 1;
  srand(time(NULL));



  printf("brick placed\r\n");
  return flag;
}

void handle_playing_state(void) {
  puts("Hints: Spin the nob to adjust the speed and difficulty\r\n"); // print on console
  LCD_Clear();
  centered_puts("SPIN THE NOB TO CHANGE SPEED", CONSOLE_WIDTH); // print on lcd
  HAL_Delay(1000);
  LCD_Clear();
  LCD_DrawString(6, 45, "3", 1);
  HAL_Delay(1000);
  LCD_Clear();
  LCD_DrawString(6, 45, "2", 1);
  HAL_Delay(1000);
  LCD_Clear();
  LCD_DrawString(6, 45, "1", 1);
  HAL_Delay(1000);
  LCD_Clear();
  LCD_DrawString(6, 45, "GO", 2);
  HAL_Delay(1000);
  LCD_Clear();

  int flag = 1;
  while(flag) {
    session.score += 1;
    // TODO 生成磚塊
    // function of brick_spawning
    brick_spawning();
    GPIO_PinState isPressed;
    do {
      isPressed = !HAL_GPIO_ReadPin(User_Button_GPIO_Port, User_Button_Pin);
    } while (!isPressed);
    // TODO 偵測按鈕是否被按下 放置磚塊 是否放置成功 否 把 flag 調成 0
    // function of brick_place return type integer
    flag = brick_place();
  }

  // 離開迴圈即失敗

  centered_puts("GAMEOVER", CONSOLE_WIDTH);
  HAL_Delay(1000);
  // TODO printout the score at the middle of the screen
  centered_puts("Score: ", CONSOLE_WIDTH);
  HAL_Delay(10000);
  LCD_Clear();
  // TODO 初始化設定 或 儲存紀錄 讓玩家進入第二次遊玩
}

void handle_paused_state(void) {
  LCD_Clear();
  GPIO_PinState isPressed;
  do {
    isPressed = HAL_GPIO_ReadPin(User_Button_GPIO_Port, User_Button_Pin);
  } while (!isPressed);
  session.state = STATE_PLAYING;
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
