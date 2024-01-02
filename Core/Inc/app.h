#ifndef APP_H
#define APP_H

#include "stm32f1xx_hal.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Configuration

#define DEBUG false
/** Starting brick is 3/4 of the screen width */
#define DEFAULT_BRICK_WIDTH (SCREEN_WIDTH * 3 / 4)
/** Starting brick is in the middle of the screen */
#define DEFAULT_BRICK_POSITION ((SCREEN_WIDTH - DEFAULT_BRICK_WIDTH) / 2)

#if DEBUG
/** printf that only activates in debug */
#define debug_printf(...) printf(__VA_ARGS__)
#else
/** printf that only activates in debug. */
#define debug_printf(...)
#endif

/** UART console width for centering text. */
#define CONSOLE_WIDTH 80

// Enums and Structs

/** Viewable bricks of the stack. */
typedef struct {
  /** The horizontal coordinate of the beginning of the brick.
   *  Can be negative to represent the invisible parts of the display.
   */
  int8_t position;
  /** The width of the brick in pixels */
  uint8_t width;
} Brick;

/** Moving direction of newly spawned brick. */
typedef enum {
  TO_RIGHT,
  TO_LEFT,
} BrickDirection;

/** Represents a button, no button, or any button.
 *  Used by the await_button function.
 */
typedef enum {
  BTN_NONE = 0,
  BTN_KEY,
  BTN_JOY,
  /** Value to listen to any button pressed above.
   *  Also represents the number of buttons.
   */
  BTN_ANY,
} GameButton;

/** Port, pin and active_state about a button used by the await_button function. */
typedef struct {
  GPIO_TypeDef *port;
  uint16_t pin;
  GPIO_PinState active_state;
} GameButtonInfo;

// Public functions

/** Code to run once at the start of the program. */
void setup(void);

/** Code to run repeatedly until hard reset. */
void loop(void);

/**
 * @brief TIM6 Global Interrupt Service Routine
 *
 * This function runs every `htim6.Init.Period / 10` milliseconds.
 * @see MX_TIM6_Init()
 */
void on_tim6(void);

/**
 * @brief TIM7 Global Interrupt Service Routine
 *
 * This function runs every `htim7.Init.Period` microseconds.
 * @see MX_TIM7_Init()
 */
void on_tim7(void);

/**
 * @brief EXTI line[15:10] Interrupts Service Routine
 *
 * Pressing the `User_Button` (Labeled "Key" on the PCB)
 * or the `JOY_SEL` (Pressing down the joystick)
 * triggers EXTI10, in which triggers this function.
 */
void on_button_press(void);

// Game tasks

/** Task while selecting difficulty and waiting for button press. */
void draw_gauge_needle(void);

/** Task while waiting for player to place a brick. */
void move_brick(void);

/** Cut the current brick,
 *  move the stack if needed,
 *  alternate brick direction
 *  and increase the score and difficulty.
 */
void place_brick(void);

/** Display the bricks on the LCD page 3 through 7. */
void display_bricks(void);

/** Blank the LCD and show "PAUSED" on the display and wait for unpause. */
void pause_game(void);

/** Prepare game global variables for a new game. */
void reset_game(void);

// Hardware utilities

/**
 * Reads raw value from ADC.
 * @return voltage read from the ADC connected to the potentiometer.
 */
uint32_t read_adc(void);

/**
 * Print a line centered in the console.
 * @param s The string to print.
 * @param width The width of the console.
 * @return Whatever puts(s) returns.
 */
int centered_puts(const char *s, uint8_t width);

/**
 * Print a line centered in the LCD.
 * @param string The string to draw.
 * @param font The font used to measure the center and to draw the text.
 * @param spacing The gap in pixels between letters
 */
void centered_draw_text(char *string, const unsigned char *font, unsigned char spacing);

/**
 * Detect for button presses by resetting a global variable, and wait for changes from the ISR.
 * The function will return when the button is released.
 * @param target The button to wait for. Use BTN_ANY to detect multiple buttons.
 * @param task Task to do while waiting for button press. This won't be called when waiting for release.
 * @return The button pressed.
 */
GameButton await_button(GameButton target, void (*task)());

// Globals

extern UART_HandleTypeDef huart1;
extern ADC_HandleTypeDef hadc1;
extern TIM_HandleTypeDef htim6, htim7;

#endif // !APP_H
