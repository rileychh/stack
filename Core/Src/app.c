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
int is_bottom_pressed = 0;
extern unsigned char HAND_DRAWN_THREE[];

uint8_t x_position, y_position;

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

void on_tim6(void) {
  HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);
}

void on_user_button_press(void) {
  is_bottom_pressed = 1;
  printf("button_pressed\r\n");
}

void handle_init_state(void) {
  puts(GAME_BANNER);
  centered_puts("Press [Key] to start the game", CONSOLE_WIDTH);

  // LCD
  LCD_DrawString(4, 45, "Start", 5); // 假设 "Start" 在第100行显示
  is_bottom_pressed = 0;
  while(is_bottom_pressed == 0){
    printf("Still in the loop\r\n");
    HAL_Delay(100);
//    if(is_bottom_pressed == 1) {
//      printf("it breaks\r\n");
//      break;
//    }
  }
  is_bottom_pressed = 0;
  session.state = STATE_PLAYING;
}

void brick_spawning(void) {
  x_position = 1;
  y_position = 0;
  is_bottom_pressed = 0;
  while(y_position < 85 && !is_bottom_pressed){
    LCD_DrawString(x_position, y_position, "xxxxx", 5);

    HAL_Delay(500);
    LCD_Clear();
    y_position += 12;

  }
  while(y_position != 0 && !is_bottom_pressed) {
    y_position -= 12;
    LCD_DrawString(x_position, y_position, "xxxxx", 5);
    HAL_Delay(500);
    LCD_Clear();
  }
  if (is_bottom_pressed == 1) {
    is_bottom_pressed = 0;
    brick_place();
  }
}

int brick_place(void) {
  int flag = 1;
  while(x_position <= 6){
    LCD_DrawString(x_position, y_position, "xxxxx", 5);
    HAL_Delay(500);
    LCD_Clear();
    x_position += 1;
  }

  return flag;
}

void handle_playing_state(void) {
  puts("Hints: Spin the nob to adjust the speed and difficulty\r\n"); // print on console
  LCD_Clear();
  centered_puts("SPIN THE NOB TO CHANGE SPEED", CONSOLE_WIDTH); // print on lcd
  HAL_Delay(1000);
  LCD_Clear();
  LCD_draw_fb(HAND_DRAWN_THREE);
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

  int is_placed = 1;
  while(is_placed) {
    session.score += 1;
    // TODO 生成磚塊
    // function of brick_spawning
    is_bottom_pressed = 0;
    brick_spawning();

    // TODO 偵測按鈕是否被按下 放置磚塊 是否放置成功 否 把 is_placed 調成 0
    // function of brick_place return type integer
//    is_placed = brick_place();
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

typedef struct {
  uint8_t position;
  uint8_t length;
} Brick;

/**
 * Draw bricks from the button of the LCD to the top.
 * @param bricks
 * @param len
 */
void draw_bricks(Brick *bricks, uint8_t len) {

}
