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
  "                   Copyright (C) 2024 Arthur, Riley and Wei    \r\n";

uint16_t difficulty = 0;                   // Speed of the moving brick, increases over time
uint16_t score = 0;                        // Number of bricks placed, up to 999
bool game_over = false;                    // Whether to continue the game
GameButtonInfo button_info[BTN_ANY];       // Info used by await button()
Brick bricks[5];                           // [0] is the moving brick, while [4] is the bottom most brick
Brick *current_brick = &bricks[3];         // Current (moving) brick is above last brick
BrickDirection brick_direction = TO_RIGHT; // First brick enters from the left of the display
bool tick = true;                          // Game tick toggled by TIM7

volatile GameButton button_pressed = BTN_NONE;

void setup() {
  HAL_TIM_Base_Start_IT(&htim6);
  HAL_TIM_Base_Start_IT(&htim7);

  glcd_init();
  glcd_blank();

  button_info[BTN_KEY].port = User_Button_GPIO_Port;
  button_info[BTN_KEY].pin = User_Button_Pin;
  button_info[BTN_KEY].active_state = GPIO_PIN_RESET;

  button_info[BTN_JOY].port = JOY_SEL_GPIO_Port;
  button_info[BTN_JOY].pin = JOY_SEL_Pin;
  button_info[BTN_JOY].active_state = GPIO_PIN_RESET;

  reset_game();
}

void loop() {
  puts(GAME_BANNER);
  centered_puts("Press [Key] to start the game", CONSOLE_WIDTH);
  for (uint8_t page = 0; page < 8; page++) {
    uint16_t offset = page * 128;
    glcd_page(page, SPLASH_SCREEN + offset);
  }
  glcd_refresh();
  await_button(BTN_KEY, NULL);
  glcd_blank();

  puts("");
  centered_puts("DIFFICULTY SELECTION", CONSOLE_WIDTH);
  puts("[Arrow] Adjust the starting difficulty");
  puts("[Key] Confirm and start the game");

  // Display the speed gauge UI on the LCD
  glcd_page(4, SPEED_GAUGE + 128); // The "V [key]" prompt
  glcd_refresh();
  await_button(BTN_KEY, draw_gauge_needle);
  glcd_blank();

  debug_printf("The difficulty is set to %u\r\n", difficulty);

  puts("");
  centered_puts("GAME START", CONSOLE_WIDTH);
  puts("[Joy] Place a brick onto the stack");
  puts("[Key] Pause the game");
  char *countdown[] = {"3", "2", "1", "GO", NULL};
  for (uint8_t i = 0; countdown[i] != NULL; i++) {
    draw_text(countdown[i], 63, 31, (unsigned char *)ArialBlack14, 0);
    glcd_refresh();
    HAL_Delay(1000);
    glcd_blank();
  }

  do {
    // Find where the last brick is from top to bottom
    Brick *last_brick = &bricks[4]; // Default to the bottom
    for (uint8_t i = 1; i < 5; i++) {
      if (bricks[i].width != 0) {
        last_brick = &bricks[i];
        break;
      }
    }

    // Current brick is above the last brick
    current_brick = last_brick - 1;

    // Spawn a new brick
    current_brick->width = last_brick->width;
    if (brick_direction == TO_RIGHT) {
      current_brick->position = 1 - current_brick->width;
    } else { // TO_LEFT
      current_brick->position = SCREEN_WIDTH - 1;
    }
    debug_printf(
      "Spawning a %upx wide brick at %d\r\n",
      current_brick->width,
      current_brick->position
    );

    switch (await_button(BTN_ANY, move_brick)) {
    case BTN_JOY:
      place_brick();
      break;
    case BTN_KEY:
      pause_game();
      break;
    default: break;
    }
  } while (!game_over);

  puts("");
  centered_puts("GAME OVER", CONSOLE_WIDTH);
  printf(
    "You stacked %u bricks. %s\r\n",
    score,
    score > 10 ? "Good Job!" : "\r\nTip: you have to look at the display to get a high score."
  );

  // TODO Leaderboard

  // Reset game state
  reset_game();
}

// This function turns on LED2 if DEBUG is true.
// Interval: 250ms
void on_tim6() {
}

// This function turns on LED3 if DEBUG is true.
// Interval: 50us
void on_tim7() {
  static int counter = 0;
  int tick_interval = 512 - difficulty * 2;
  if (tick_interval < 0) tick_interval = 0;

  if (counter >= tick_interval) {
    tick = true;
    counter = 0;
  }
  counter++;
}

// This function turns on LED4 if DEBUG is true.
void on_button_press() {
  if (button_pressed == BTN_NONE) {
    if (HAL_GPIO_ReadPin(User_Button_GPIO_Port, User_Button_Pin) == GPIO_PIN_RESET)
      button_pressed = BTN_KEY;
    else if (HAL_GPIO_ReadPin(JOY_SEL_GPIO_Port, JOY_SEL_Pin) == GPIO_PIN_RESET)
      button_pressed = BTN_JOY;
  }
}

void draw_gauge_needle() {
  difficulty = read_adc() >> 5; // Map ADC value (0 - 4095) to horizontal position (0 - 127)
  glcd_page(3, SPEED_GAUGE);    // The gauge

  // Draw a 3px wide needle
  for (int column = difficulty - 1; column <= difficulty + 1; column++) {
    glcd_column(3, column, 0xff);
  }
  glcd_refresh();
}

void move_brick() {
  if (!tick) return;

  if (current_brick->position >= SCREEN_WIDTH - 1) {
    brick_direction = TO_LEFT;
  } else if (current_brick->position <= 1 - current_brick->width) {
    brick_direction = TO_RIGHT;
  }

  if (brick_direction == TO_RIGHT) {
    current_brick->position++;
  } else { // TO_LEFT
    current_brick->position--;
  }

  debug_printf(
    "Moved brick %s to %d\r\n",
    brick_direction == TO_RIGHT ? "right" : "left",
    current_brick->position
  );

  display_bricks();
  tick = false;
}

void place_brick() {
  // Cut the brick to fit above the last brick.
  // They have the same width, so we only have to check the position.
  Brick *last_brick = current_brick + 1;
  int8_t current_start = current_brick->position,
         current_end = current_start + current_brick->width,
         last_start = last_brick->position,
         last_end = last_start + last_brick->width;

  if (current_end <= last_start || current_start >= last_end) {
    // Did not overlap with the last brick
    game_over = true;
    return;

  } else if (current_start < last_start) {
    // Placed to the left of the last brick
    current_brick->width -= last_start - current_start;
    current_brick->position = last_start;

  } else if (current_start > last_start) {
    // Placed to the right of the last brick
    current_brick->width -= current_start - last_start;

  } else {
    // Placed on top of the last brick
    puts("PERFECT placement!");
    score++; // Get an extra point

    // Grow the current brick
    current_brick->position--;
    current_brick->width += 2;
  }

  // Move the stack down by one if stacked to the top
  if (current_brick == &bricks[0]) {
    for (int i = 4; i > 0; i--) {
      bricks[i] = bricks[i - 1];
    }
    bricks[0] = (Brick) {0};
  }

  // Alternate the incoming direction of the bricks
  if (brick_direction == TO_RIGHT) brick_direction = TO_LEFT;
  else /* TO_LEFT */ brick_direction = TO_RIGHT;

  score++;
  difficulty += 2;
}

void display_bricks() {
  for (uint8_t i = 0; i < 5; i++) {
    uint8_t page = i + 3, start, end;

    // Skip zero-width bricks
    if (bricks[i].width == 0) continue;

    // Check if the sides are out of bounds
    bool is_left_oob = false, is_right_oob = false;

    if (bricks[i].position >= 0)
      start = bricks[i].position;
    else {
      start = 0;
      is_left_oob = true;
    }

    if (bricks[i].position + bricks[i].width < SCREEN_WIDTH)
      end = bricks[i].position + bricks[i].width;
    else {
      end = SCREEN_WIDTH - 1;
      is_right_oob = true;
    }

    debug_printf("[%u: %u-%u] \r\n", i, start, end);

    static unsigned char empty_page[SCREEN_WIDTH] = {0};
    glcd_page(page, empty_page);

    for (uint8_t column = start; column <= end; column++) {
      uint8_t data;
      if (
        !is_left_oob && column == start ||
        !is_right_oob && column == end
      ) data = 0xff;                    // left and right border
      else if (column % 2) data = 0xab; // 0xaa but with bottom border
      else data = 0xd5;                 // 0x55 but with top border
      glcd_column(page, column, data);
    }
  }

  debug_printf("\r\n");
  glcd_refresh();
}

void pause_game() {
  glcd_blank();
  draw_text("PAUSED", 63, 31, (unsigned char *)ArialBlack14, 2);
  glcd_refresh();

  // Clear the current brick to correct last brick detection
  *current_brick = (Brick) {0};

  await_button(BTN_KEY, NULL);
  glcd_blank();
}

void reset_game(void) {
  game_over = false;
  score = 0;
  brick_direction = TO_RIGHT;
  tick = true;

  for (int i = 0; i < 5; i++) {
    bricks[i] = (Brick) {0};
  }
  bricks[4].width = DEFAULT_BRICK_WIDTH;
  bricks[4].position = DEFAULT_BRICK_POSITION;
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

void centered_draw_text(char *string, const unsigned char *font, unsigned char spacing) {
  unsigned char x = (SCREEN_WIDTH - text_width(string, font, spacing)) / 2;
  unsigned char y = (SCREEN_HEIGHT - text_height(string, font)) / 2;
  draw_text(string, x, y, font, spacing);
}

/** Helper function to determine if a button is pressed. */
bool is_pressed(GameButton target) {
  if (target == BTN_ANY) return button_pressed != BTN_NONE;
  return button_pressed == target;
}

GameButton await_button(GameButton target, void (*task)()) {
  do {
    button_pressed = BTN_NONE;
    if (task) task();
  } while (!is_pressed(target));

  debug_printf("Button %u is pressed\r\n", button_pressed);

  GameButtonInfo btn = button_info[button_pressed];
  HAL_Delay(50); // Debounce
  while (HAL_GPIO_ReadPin(btn.port, btn.pin) == btn.active_state) {
    // Wait for release
  }
  HAL_Delay(50); // Debounce

  return button_pressed;
}
