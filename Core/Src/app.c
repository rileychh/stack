#include "app.h"

#include "Res/font_arialblack.h"
#include "Res/glcd_images.h"
#include "glcd.h"
#include "graphics.h"
#include "main.h"
#include "uart_stdio.h"

#include <stdbool.h>
#include <string.h>

const char *GAME_BANNER =
  "\r\n"
  "                 _|_|_|    _|                          _|      \r\n"
  "               _|        _|_|_|_|    _|_|_|    _|_|_|  _|  _|  \r\n"
  "                 _|_|      _|      _|    _|  _|        _|_|    \r\n"
  "                     _|    _|      _|    _|  _|        _|  _|  \r\n"
  "               _|_|_|        _|_|    _|_|_|    _|_|_|  _|    _|\r\n"
  "                   Copyright (C) 2023 Arthur, Riley and Wei    \r\n";

GameState state;    // State of the game
uint8_t difficulty; // Speed of the moving brick
size_t score;       // Number of bricks placed
GameButtonInfo button_info[4];

/** [0] is the moving brick, while [4] is the bottom most brick. */
Brick bricks[5];

volatile GameButton button_pressed = BTN_NONE;

void setup() {
  glcd_init();
  glcd_blank();

  score = 0;
  state = STATE_INIT;
  button_info[BTN_KEY].port = User_Button_GPIO_Port;
  button_info[BTN_KEY].pin = User_Button_Pin;
  button_info[BTN_KEY].active_state = GPIO_PIN_RESET;

  button_info[BTN_JOY].port = JOY_SEL_GPIO_Port;
  button_info[BTN_JOY].pin = JOY_SEL_Pin;
  button_info[BTN_JOY].active_state = GPIO_PIN_RESET;

  HAL_TIM_Base_Start_IT(&htim6);
}

void loop() {
  switch (state) {
  case STATE_INIT: handle_init_state(); break;
  case STATE_PLAYING: handle_playing_state(); break;
  case STATE_PAUSED: handle_paused_state(); break;
  }
}

// This function turns on LED2 if DEBUG is true.
void on_tim6(void) {
}

// This function turns on LED3 if DEBUG is true.
void on_button_press(void) {
  if (button_pressed == BTN_NONE) {
    if (HAL_GPIO_ReadPin(User_Button_GPIO_Port, User_Button_Pin) == GPIO_PIN_RESET)
      button_pressed = BTN_KEY;
    else if (HAL_GPIO_ReadPin(JOY_SEL_GPIO_Port, JOY_SEL_Pin) == GPIO_PIN_RESET)
      button_pressed = BTN_JOY;
  }
}

/** Task while selecting difficulty and waiting for button press. */
void draw_gauge_needle() {
  difficulty = read_adc() >> 5; // Map ADC value (0 - 4095) to LCD x position (0 - 127)
  glcd_page(3, SPEED_GAUGE); // The gauge

  // Draw a 3px wide needle
  for (int column = difficulty - 1; column <= difficulty + 1; column++) {
    glcd_column(3, column, 0xff);
  }
  glcd_refresh();
}

void handle_init_state(void) {
  puts(GAME_BANNER);
  centered_puts("Press [Key] to start the game", CONSOLE_WIDTH);
  for (uint8_t page = 0; page < 8; page++) {
    uint16_t offset = page * 128;
    glcd_page(page, SPLASH_SCREEN + offset);
  }
  glcd_refresh();
  await_button(BTN_KEY, NULL);
  glcd_blank();

  centered_puts("Spin the nob to adjust the speed and difficulty", CONSOLE_WIDTH);

  // Display the speed gauge UI on the LCD
  glcd_page(4, SPEED_GAUGE + 128); // The "V [key]" prompt
  glcd_refresh();
  await_button(BTN_KEY, draw_gauge_needle);
  glcd_blank();

  char *countdown[] = {"3", "2", "1", "GO", NULL};
  for (uint8_t i = 0; countdown[i] != NULL; i++) {
    draw_text(countdown[i], 63, 31, (unsigned char *)ArialBlack14, 0);
    glcd_refresh();
    HAL_Delay(1000);
    glcd_blank();
  }
  state = STATE_PLAYING;
}

void handle_playing_state(void) {
  // Initialize the bricks
  for (int i = 0; i < 5; i++) {
    bricks[i] = (Brick) {0};
  }

  bricks[0].position = 0;
  bricks[0].width = 16;

  bricks[1].position = 0;
  bricks[1].width = 17;

  bricks[2].position = 0;
  bricks[2].width = 18;

  bricks[3].position = 0;
  bricks[3].width = 19;

  bricks[4].position = 0;
  bricks[4].width = 20;

  display_bricks();
#if 0
  LCD_Clear();
  bool is_placed = true;
  while (is_placed) {
    score += 1;
    // TODO 生成磚塊
    // function of brick_spawning
    is_bottom_pressed = 0;
    brick_spawning();

    // TODO 偵測按鈕是否被按下 放置磚塊 是否放置成功 否 把 is_placed 調成 0
    // function of brick_place return type integer
    // is_placed = brick_place();
  }

  // 離開迴圈即失敗

  centered_puts("GAME OVER", CONSOLE_WIDTH);
  HAL_Delay(1000);
  // TODO printout the score at the middle of the screen
  centered_puts("Score: ", CONSOLE_WIDTH);
  HAL_Delay(10000);
  LCD_Clear();
  // TODO 初始化設定 或 儲存紀錄 讓玩家進入第二次遊玩
#endif
}

void handle_paused_state(void) {
  glcd_blank();
  draw_text("PAUSED", 63, 31, (unsigned char *)ArialBlack14, 2);
  glcd_refresh();
  await_button(BTN_KEY, NULL);
  state = STATE_PLAYING;
}

#if 0
int x_position, y_position, is_bottom_pressed;

void brick_spawning(void) {
  x_position = 1;
  y_position = 0;
  is_bottom_pressed = 0;
  while (y_position < 85 && !is_bottom_pressed) {
    LCD_DrawString(x_position, y_position, "xxxxx", 5);

    HAL_Delay(500);
    LCD_Clear();
    y_position += 12;
  }

  while (y_position != 0 && !is_bottom_pressed) {
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
  while (x_position <= 6) {
    LCD_DrawString(x_position, y_position, "xxxxx", 5);
    HAL_Delay(500);
    LCD_Clear();
    x_position += 1;
  }

  return flag;
}
#endif

void display_bricks() {
  for (uint8_t i = 0; i < 5; i++) {
    uint8_t page = i + 3;
    uint8_t start = bricks[i].position, end = start + bricks[i].width;
    for (uint8_t column = start; column <= end; column++) {
      uint8_t data;
      if (column == start || column == end) data = 0xff; // left and right border
      else if (column % 2) data = 0xab;                  // 0xaa but with bottom border
      else data = 0xd5;                                  // 0x55 but with top border
      glcd_column(page, column, data);
    }
  }

  glcd_refresh();
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

bool is_pressed(GameButton target) {
  if (target == BTN_ANY) return button_pressed != BTN_NONE;
  return button_pressed == target;
}

GameButton await_button(GameButton target, void (*task)()) {
  button_pressed = BTN_NONE;

  while (!is_pressed(target)) {
    if (task) task();
  }
  debug_printf("A button is pressed\r\n");

  GameButtonInfo btn = button_info[button_pressed];
  HAL_Delay(20); // Debounce
  while (HAL_GPIO_ReadPin(btn.port, btn.pin) == btn.active_state) {
    // Wait for release
  }
  HAL_Delay(20); // Debounce

  return button_pressed;
}
