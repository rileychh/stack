#ifndef GLCD_H
#define GLCD_H

#include "stm32f1xx.h"

typedef __IO struct {
  uint8_t cmd;  // A0=0
  uint8_t data; // A0=1
} lcd_t;
#define LCD ((lcd_t *)FSMC_BANK1_4)

/** Screen width in pixels */
#define SCREEN_WIDTH 128

/** Screen height in pixels */
#define SCREEN_HEIGHT 64

/** Define this if your screen is incorrectly shifted by 4 pixels */
// #define ST7565_REVERSE

/** By default we only write pages that have changed.  Undefine this
    if you want less/faster code at the expense of more SPI operations. */
#define ST7565_DIRTY_PAGES 1

/** Command: turn the display on */
#define GLCD_CMD_DISPLAY_ON 0xAF

/** Command: turn the display off */
#define GLCD_CMD_DISPLAY_OFF 0xAE

/** Command: set all points on the screen to normal. */
#define GLCD_CMD_ALL_NORMAL 0xA4

/** Command: set all points on the screen to "on", without affecting
       the internal screen buffer. */
#define GLCD_CMD_ALL_ON 0xA5

/** Command: disable inverse (black pixels on a white background) */
#define GLCD_CMD_DISPLAY_NORMAL 0xA6

/** Command: inverse the screen (white pixels on a black background) */
#define GLCD_CMD_DISPLAY_REVERSE 0xA7

/** Command: set LCD bias to 1/9th */
#define GLCD_CMD_BIAS_9 0xA2
/** Command: set LCD bias to 1/7th */
#define GLCD_CMD_BIAS_7 0xA3

/** Command: set ADC output direction to normal. */
#define GLCD_CMD_HORIZONTAL_NORMAL 0xA0

/** Command: set ADC output direction reverse (horizontally flipped).
       Note that you should use the glcd_flip_screen function so that
       the width is correctly accounted for. */
#define GLCD_CMD_HORIZONTAL_REVERSE 0xA1

/** Command: set common output scan direction to normal. */
#define GLCD_CMD_VERTICAL_NORMAL 0xC0

/** Command: set common output scan direction to reversed (vertically flipped). */
#define GLCD_CMD_VERTICAL_REVERSE 0xC8

/** Command: select the internal power supply operating mode. */
#define GLCD_CMD_POWER_CONTROL 0x28

/** Command: set internal R1/R2 resistor bias (OR with 0..7) */
#define GLCD_CMD_RESISTOR 0x20

/** Command: enter volume mode, send this then send another command
       byte with the contrast (0..63).  The second command
			 must be sent for the GLCD to exit volume mode. */
#define GLCD_CMD_VOLUME_MODE 0x81

#define GLCD_CMD_DISPLAY_START 0x40

/** Command: set the least significant 4 bits of the column address. */
#define GLCD_CMD_COLUMN_LOWER 0x00

/** Command: set the most significant 4 bits of the column address. */
#define GLCD_CMD_COLUMN_UPPER 0x10

/** Command: Set the current page (0..7). */
#define GLCD_CMD_SET_PAGE 0xB0

/** Command: software reset (note: should be combined with toggling GLCD_RS) */
#define GLCD_CMD_RESET 0xE2

/** Command: no operation (note: the datasheet suggests sending this periodically
       to keep the data connection alive) */
#define GLCD_CMD_NOP 0xE3

/**
 * Initialise the screen.  This should be called first.
 */
void glcd_init(void);

/**
 * Send a command byte to the screen.  See GLCD_CMD_ constants for
 * a list of commands.
 */
void glcd_command(char);

/**
 * Send a data byte to the screen.
 */
void glcd_data(char);

/**
 * Update the screen with the contents of the RAM buffer.
 */
void glcd_refresh(void);

/**
 * Clear the screen, without affecting the buffer in RAM.
 *
 * Useful at startup as the memory inside the screen may
 * contain "random" data.
 */
void glcd_blank(void);

/**
 * Set a single pixel
 *
 * @param x 		The x position, from 0 - SCREEN_WIDTH-1
 * @param y 		The y position, from 0 - SCREEN_HEIGHT-1
 * @param colour 	0 = OFF, any other value = ON
 */
void glcd_pixel(unsigned char x, unsigned char y, unsigned char colour);

/**
 * Set a 8-pixel high column of a page. This is faster then glcd_pixel.
 * @param page The page number, from 0 - 7
 * @param column The column, from 0 - SCREEN_WIDTH-1
 * @param colours The state of the 8 pixels
 */
void glcd_column(unsigned char page, unsigned char column, unsigned char colours);

/**
 * Write to all columns of a page.
 * @param page The page number, from 0 - 7
 * @param data An array with a length equal to SCREEN_WIDTH, containing all columns.
 */
void glcd_page(unsigned char page, unsigned char *data);

/**
 * Flip the screen in the alternate direction vertically.
 *
 * Can be used if the screen is mounted in an enclosure upside
 * down.
 */
void glcd_flip_screen(unsigned char flip);

/**
 * Inverse the screen, swapping "on" and "off" pixels.
 *
 * This does not affect the RAM buffer or the screen memory, the controller
 * is capable of reversing pixels with a single command.
 */
void glcd_inverse_screen(unsigned char inverse);

/**
 * Fill the local RAM buffer with a test pattern and send it to the screen.
 *
 * Useful for ensuring that the screen is receiving data correctly or for
 * adjusting contrast.
 */
void glcd_test_card(void);

/**
 * Set the contrast of the screen.  This involves two steps, setting the
 * internal resistor ratio (R1:R2) and then the contrast.
 *
 * Tip: Find a resistor ratio that works well with the screen and stick to it
 *      throughout.  Then adjust the contrast dynamically between 0 and 63.
 *
 * @param resistor_ratio	Ratio of the internal resistors, from 0-7
 * @param contrast			Contrast, from 0-63
 */
void glcd_contrast(char resistor_ratio, char contrast);

#endif // GLCD_H
