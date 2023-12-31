#include "glcd.h"

/** Global buffer to hold the current screen contents. */
unsigned char glcd_buffer[SCREEN_WIDTH * SCREEN_HEIGHT / 8];

/** Global variable that tracks whether the screen is the "normal" way up. */
unsigned char glcd_flipped = 0;

#ifdef ST7565_DIRTY_PAGES
unsigned char glcd_dirty_pages;
#endif

void glcd_command(char cmd) {
  LCD->cmd = cmd;
}

void glcd_data(char data) {
  LCD->data = data;
}

void glcd_pixel(unsigned char x, unsigned char y, unsigned char colour) {

  if (x > SCREEN_WIDTH || y > SCREEN_HEIGHT) return;

  // Real screen coordinates are 0-63, not 1-64.
  x -= 1;
  y -= 1;

  unsigned short array_pos = x + ((y / 8) * 128);

#ifdef ST7565_DIRTY_PAGES
#warning ** ST7565_DIRTY_PAGES enabled, only changed pages will be written to the GLCD **
  glcd_dirty_pages |= 1 << (array_pos / 128);
#endif

  if (colour) {
    glcd_buffer[array_pos] |= 1 << (y % 8);
  } else {
    glcd_buffer[array_pos] &= 0xFF ^ 1 << (y % 8);
  }
}

void glcd_blank() {
  // Reset the internal buffer
  for (int n = 1; n <= (SCREEN_WIDTH * SCREEN_HEIGHT / 8) - 1; n++) {
    glcd_buffer[n] = 0;
  }

  // Clear the actual screen
  for (int y = 0; y < 8; y++) {
    glcd_command(GLCD_CMD_SET_PAGE | y);

    // Reset column to 0 (the left side)
    glcd_command(GLCD_CMD_COLUMN_LOWER);
    glcd_command(GLCD_CMD_COLUMN_UPPER);

    // We iterate to 132 as the internal buffer is 65*132, not
    // 64*124.
    for (int x = 0; x < 132; x++) {
      glcd_data(0x00);
    }
  }
}

void glcd_refresh() {
  for (int y = 0; y < 8; y++) {

#ifdef ST7565_DIRTY_PAGES
    // Only copy this page if it is marked as "dirty"
    if (!(glcd_dirty_pages & (1 << y))) continue;
#endif

    glcd_command(GLCD_CMD_SET_PAGE | y);

    // Reset column to the left side.  The internal memory of the
    // screen is 132*64, we need to account for this if the display
    // is flipped.
    //
    // Some screens seem to map the internal memory to the screen
    // pixels differently, the ST7565_REVERSE define allows this to
    // be controlled if necessary.
#ifdef ST7565_REVERSE
    if (!glcd_flipped) {
#else
    if (glcd_flipped) {
#endif
      glcd_command(GLCD_CMD_COLUMN_LOWER | 4);
    } else {
      glcd_command(GLCD_CMD_COLUMN_LOWER);
    }
    glcd_command(GLCD_CMD_COLUMN_UPPER);

    for (int x = 0; x < 128; x++) {
      glcd_data(glcd_buffer[y * 128 + x]);
    }
  }

#ifdef ST7565_DIRTY_PAGES
  // All pages have now been updated, reset the indicator.
  glcd_dirty_pages = 0;
#endif
}

void glcd_init() {
  glcd_command(GLCD_CMD_RESET);

  // Set LCD bias to 1/9th
  glcd_command(GLCD_CMD_BIAS_9);

  // Horizontal output direction (ADC segment driver selection)
  glcd_command(GLCD_CMD_HORIZONTAL_NORMAL);

  // Vertical output direction (common output mode selection)
  glcd_command(GLCD_CMD_VERTICAL_REVERSE);

  // Set internal resistor.  A suitable middle value is used as
  // the default.
  glcd_command(GLCD_CMD_RESISTOR | 0x5);

  // Power control setting (datasheet step 7)
  glcd_command(GLCD_CMD_POWER_CONTROL | 0x7);

  // Volume set (brightness control).  A middle value is used here also.
  glcd_command(GLCD_CMD_VOLUME_MODE);
  glcd_command(31);

  // Reset start position to the top
  glcd_command(GLCD_CMD_DISPLAY_START);

  // Turn the display on
  glcd_command(GLCD_CMD_DISPLAY_ON);
}

void glcd_inverse_screen(unsigned char inverse) {
  if (inverse) {
    glcd_command(GLCD_CMD_DISPLAY_REVERSE);
  } else {
    glcd_command(GLCD_CMD_DISPLAY_NORMAL);
  }
}

void glcd_test_card() {
  unsigned char p = 0xF0;

  for (int n = 1; n <= (SCREEN_WIDTH * SCREEN_HEIGHT / 8); n++) {
    glcd_buffer[n - 1] = p;

    if (n % 4 == 0) {
      unsigned char q = p;
      p = p << 4;
      p |= q >> 4;
    }
  }

  glcd_refresh();
}

void glcd_contrast(char resistor_ratio, char contrast) {
  if (resistor_ratio > 7 || contrast > 63) return;

  glcd_command(GLCD_CMD_RESISTOR | resistor_ratio);
  glcd_command(GLCD_CMD_VOLUME_MODE);
  glcd_command(contrast);
}
