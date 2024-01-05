#include "app.h"

#include "Res/font_arialblack.h"
#include "Res/font_lucidaconsole.h"
#include "Res/glcd_images.h"
#include "glcd.h"
#include "graphics.h"
#include "main.h"
#include "uart_stdio.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
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
Brick *current_brick = &bricks[3];         // Current (moving) brick is 1 page above the last brick
BrickDirection brick_direction = TO_RIGHT; // First brick enters from the left of the display
HighScore high_score = {0};                // The highest score achieved
bounding_box_t score_box = {0};            // The bounding box of the score drawn on the LCD
bool game_tick = true;                     // Game tick toggled by TIM7
bool needs_refresh = true;                 // Display refresh tick toggled by TIM10
uint8_t duty_cycles[4] = {0};              // PWM duty cycles for the LEDs

volatile GameButton button_pressed = BTN_NONE;

void setup() {
#if FPS
  HAL_TIM_Base_Start_IT(&htim2);
#endif
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

  // Set an easy-to-beat high score
  high_score.name = "RILEY";
  high_score.score = 5;

  reset_game();
}

void loop() {
  puts(GAME_BANNER);
  centered_puts("Press [Key] to start the game", CONSOLE_WIDTH);
  centered_puts("Press [Joy] to view the high score", CONSOLE_WIDTH);

  while (true) {
    for (uint8_t page = 0; page < 8; page++) {
      uint16_t offset = page * 128;
      glcd_page(page, SPLASH_SCREEN + offset);
    }
    glcd_refresh();

    char *title = "HIGH SCORE";
    uint8_t spacing = 4;
    const unsigned char *font = ArialBlack12;
    uint8_t title_x = (SCREEN_WIDTH - text_width(title, font, spacing)) / 2;
    char high_score_string[16];

    switch (await_button(BTN_ANY, led_betel_nut_stand, NULL)) {
    case BTN_KEY: goto EXIT_TITLE;
    case BTN_JOY:
      glcd_blank();

      draw_text(title, title_x, 0, font, spacing);
      snprintf(high_score_string, 16, "%03u by %8s", high_score.score, high_score.name);
      centered_draw_text(high_score_string, LucidaConsole10_Bold, 0);
      glcd_refresh();

      printf("High score: %s\r\n", high_score_string);
      puts("Press any button to return to the title screen");
      await_button(BTN_ANY, led_breathe, NULL);
    default: break;
    }
  }
EXIT_TITLE:
  glcd_blank();

  puts("");
  centered_puts("DIFFICULTY SELECTION", CONSOLE_WIDTH);
  puts("[Arrow]\tAdjust the starting difficulty");
  puts("[Key]\tConfirm and start the game");

  // Display the speed gauge UI on the LCD
  glcd_page(4, SPEED_GAUGE + 128); // The "V [key]" prompt
  glcd_refresh();
  await_button(BTN_KEY, draw_gauge_needle, NULL);
  glcd_blank();

  debug_printf("The difficulty is set to %u\r\n", difficulty);

  puts("");
  centered_puts("GAME START", CONSOLE_WIDTH);
  puts("[Joy]\tPlace a brick onto the stack");
  puts("[Key]\tPause the game");
  char *countdown[] = {"3", "2", "1", "GO", NULL};
  for (uint8_t i = 0; countdown[i] != NULL; i++) {
    centered_draw_text(countdown[i], ArialBlack16, 0);
    glcd_refresh();

    if (i == 3) {
      led_checkered_blocking();
    } else {
      for (int j = 0; j < 4; j++) {
        duty_cycles[j] = 25 * i;
      }
    }

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
      current_brick->position = (int8_t)(1 - current_brick->width);
    } else { // TO_LEFT
      current_brick->position = SCREEN_WIDTH - 1;
    }
    debug_printf(
      "Spawning a %upx wide brick at %d\r\n",
      current_brick->width,
      current_brick->position
    );

    switch (await_button(BTN_ANY, move_brick, led_on)) {
    case BTN_JOY:
      place_brick();
      display_score();
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
    "You stacked %u brick%s. %s\r\n",
    score,
    score == 1 ? "" : "s",
    score > 10 ? "Good Job!" : "\r\nTip: you have to look at the display to get a high score."
  );

  puts("");
  glcd_blank();
  if (score >= high_score.score) {
    centered_draw_text("HIGH SCORE!", ArialBlack14, 1);
    glcd_refresh();

    centered_puts("YOU BEAT THE HIGH SCORE!", CONSOLE_WIDTH);
    puts("Enter your name (up to 8 characters) and press Enter on keyboard.");
    static char name[9];
    scanf("%8s", name);
    for (int i = 0; name[i]; i++) name[i] = toupper(name[i]); // Convert to uppercase
    high_score.name = name;
    high_score.score = score;
  } else {
    centered_draw_text("GAME OVER", ArialBlack14, 1);
    glcd_refresh();
    puts("Press any button to return to the title screen");
    await_button(BTN_ANY, NULL, NULL);
  }

  reset_game();
}

// This function turns on LED1 if DEBUG is true.
void on_tim2() {
#if FPS
  const int frame_time = (1000 + FPS / 2) / FPS; // rounded to the nearest integer
  static int counter = 0;

  if (counter < frame_time) {
    counter++;
    return;
  }

  // Only reset the counter when the display is refreshed
  if (needs_refresh) {
    glcd_refresh();
    needs_refresh = false;
    counter = 0;
  }
#endif
}

// This function turns on LED2 if DEBUG is true.
void on_tim6() {
#if !DEBUG // PWM is disabled in debug mode
  static int pwm_counter = 0;

  // Set the duty cycle of LED1 to LED4
  for (int i = 0; i < 4; i++) {
    if (pwm_counter < duty_cycles[i]) {
      HAL_GPIO_WritePin(GPIOF, (uint16_t)0x40 << i, GPIO_PIN_SET);
    } else {
      HAL_GPIO_WritePin(GPIOF, (uint16_t)0x40 << i, GPIO_PIN_RESET);
    }
  }

  pwm_counter = (pwm_counter + 1) % 100;
#endif
}

// This function turns on LED3 if DEBUG is true.
void on_tim7() {
  static int counter = 0;
  int tick_interval = 512 - difficulty * 2;
  if (tick_interval < 0) tick_interval = 0;

  if (counter >= tick_interval) {
    game_tick = true;
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

  // Sync the brightness of the LEDs to the position of the needle
  uint8_t brightness = difficulty > 100 ? 100 : difficulty;
  for (int i = 0; i < 4; i++) {
    duty_cycles[i] = brightness;
  }
}

void move_brick() {
  if (!game_tick) return;

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
  game_tick = false;
}

void place_brick() {
  // Cut the brick to fit above the last brick.
  // They have the same width, so we only have to check the position.
  Brick *last_brick = current_brick + 1;
  int8_t current_start = current_brick->position;
  int16_t current_end = (int16_t)(current_start + current_brick->width);
  int8_t last_start = last_brick->position;
  int16_t last_end = (int16_t)(last_start + last_brick->width);

  debug_printf("Current: %d - %d, Last: %d - %d\r\n", current_start, current_end, last_start, last_end);

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

    // Flash the LEDs and the score box twice
    const uint8_t animation[] = {100, 0, 100, 0};
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        duty_cycles[j] = animation[i];
        draw_rectangle(score_box.x1 - j - 2, score_box.y1 - j - 1, score_box.x2 + j + 2, score_box.y2 + j - 2, animation[i]);
      }
      glcd_refresh();
      HAL_Delay(250);
    }

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
#if FPS
  if (needs_refresh) return; // Wait for display refresh
#endif

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
#if !FPS // frame-limited mode doesn't need to worry about the response time
        // display brick in black for visibility
        &bricks[i] == current_brick && current_brick->width < 16 ||
#endif
        !is_left_oob && column == start || // left border
        !is_right_oob && column == end     // right border
      )
        data = 0xff;
      else if (column % 2) data = 0xab; // 0xaa but with bottom border
      else data = 0xd5;                 // 0x55 but with top border
      glcd_column(page, column, data);
    }
  }

  debug_printf("\r\n");

#if FPS
  needs_refresh = true;
#else
  glcd_refresh();
#endif

  // Sync the LEDs to the position of the brick
  int16_t brick_center = (int16_t)(current_brick->position + current_brick->width / 2);
  uint8_t anchors[4] = {127, 96, 63, 0};
  for (int i = 0; i < 4; i++) {
    uint16_t distance = abs(brick_center - anchors[i]) * 2;
    if (distance > 100) distance = 100;
    duty_cycles[i] = 100 - distance;
  }
}

void display_score() {
  // Clear the score area
  for (uint8_t x = score_box.x1; x <= score_box.x2; x++) {
    for (uint8_t y = score_box.y1; y <= score_box.y2; y++) {
      glcd_pixel(x, y, 0);
    }
  }

  char string[4];
  snprintf(string, 4, "%03u", score);
  const unsigned char *font = ArialBlack12;
  unsigned char spacing = 8;
  unsigned char x = (SCREEN_WIDTH - text_width(string, font, spacing)) / 2;
  unsigned char y = 4; // Leave 4px of space for the perfect placement box
  score_box = draw_text(string, x, y, font, spacing);

  glcd_refresh();
}

void pause_game() {
  // Turn off all LEDs
  for (int i = 0; i < 4; i++) {
    duty_cycles[i] = 0;
  }

  centered_puts("GAME PAUSED", CONSOLE_WIDTH);
  printf("Score: %u\r\n", score);
  puts("[Key]\tResume the game");

  glcd_blank();
  centered_draw_text("PAUSED", ArialBlack16, 4);
  glcd_refresh();

  // Clear the current brick to correct last brick detection
  *current_brick = (Brick) {0};

  await_button(BTN_KEY, NULL, NULL);
  glcd_blank();
}

void reset_game(void) {
  game_over = false;
  score = 0;
  brick_direction = TO_RIGHT;
  game_tick = true;

  for (int i = 0; i < 4; i++) {
    duty_cycles[i] = 0;
  }

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

GameButton await_button(
  GameButton target,
  void (*while_wait)(),
  void (*while_hold)()
) {
  do {
    button_pressed = BTN_NONE;
    if (while_wait) while_wait();
  } while (!is_pressed(target));

  debug_printf("Button %u is pressed\r\n", button_pressed);

  GameButtonInfo btn = button_info[button_pressed];
  HAL_Delay(20); // Debounce
  while (HAL_GPIO_ReadPin(btn.port, btn.pin) == btn.active_state) {
    if (while_hold) while_hold();
  }
  HAL_Delay(20); // Debounce

  return button_pressed;
}

void led_betel_nut_stand() {
  // Animations: [0] is animation length, [1] is delay between frames in milliseconds,
  // each byte after is 2 frames for the 4 LEDs.
  const uint8_t flash[] = {2, 50, 0x0f};
  const uint8_t stack_from_left[] = {11, 75, 0x01, 0x24, 0x89, 0xac, 0xde, 0xf0};
  const uint8_t stack_from_right[] = {11, 75, 0x84, 0x42, 0x19, 0x53, 0xb7, 0xf0};
  const uint8_t grow_from_left[] = {4, 100, 0x8c, 0xef};
  const uint8_t grow_from_right[] = {4, 100, 0x13, 0x7f};

  const uint8_t *sequence[] = {
    flash,
    flash,
    flash,
    flash,
    stack_from_left,
    stack_from_right,
    flash,
    flash,
    flash,
    flash,
    grow_from_left,
    grow_from_right,
    grow_from_left,
    grow_from_right,
    NULL
  };

  // We want this function to return ASAP to avoid delaying interrupts,
  // So we track the animation in static variables and return every frame.
  static uint8_t animation_index = 0; // Index of the current animation in the sequence
  static uint8_t frame_count = 0;     // Number of frames in the current animation
  static uint8_t animation_delay = 0; // Delay between frames in milliseconds
  static uint8_t frame = 0;           // Current frame

  // Start over when the sequence ends
  if (sequence[animation_index] == NULL) {
    animation_index = 0;
    frame_count = sequence[animation_index][0];
    animation_delay = sequence[animation_index][1];
    frame = 0;
  }

  if (frame < frame_count) {
    // Extract a nibble representing the 4 LED states from the byte array
    uint8_t two_frames = sequence[animation_index][2 + frame / 2];
    uint8_t led_states = frame % 2 ? two_frames & 0x0f : two_frames >> 4;

    // Set each LED to on (100) or off (0)
    for (int i = 0; i < 4; i++) {
      duty_cycles[i] = led_states & (0x01 << i) ? 100 : 0;
    }
    HAL_Delay(animation_delay);
    frame++;
  } else {
    // Move on to the next animation
    animation_index++;
    frame_count = sequence[animation_index][0];
    animation_delay = sequence[animation_index][1];
    frame = 0;
  }
}

void led_breathe() {
  static uint8_t brightness = 0;
  static bool is_increasing = true;

  if (is_increasing) {
    if (brightness < 100) brightness++;
    else is_increasing = false;
  } else {
    if (brightness > 0) brightness--;
    else is_increasing = true;
  }

  for (int i = 0; i < 4; i++) {
    duty_cycles[i] = brightness;
  }
  HAL_Delay(10);
}

void led_on() {
  if (button_pressed == BTN_JOY) {
    for (int i = 0; i < 4; i++) {
      duty_cycles[i] = 100;
    }
  }
}

void led_checkered_blocking() {
  uint8_t led_states = 0x5;
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      duty_cycles[j] = led_states & (0x01 << j) ? 100 : 0;
    }
    led_states = ~led_states;
    HAL_Delay(250);
  }
}
