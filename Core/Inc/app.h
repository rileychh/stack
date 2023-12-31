#ifndef APP_H
#define APP_H

#include "stm32f1xx_hal.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Configuration

#define DEBUG true

#if DEBUG
#define debug_printf(...) printf(__VA_ARGS__)
#else
#define debug_printf(...)
#endif

#define CONSOLE_WIDTH 80

// Enums and Structs

typedef enum {
  STATE_INIT,
  STATE_PLAYING,
  STATE_PAUSED,
} GameState;

typedef struct {
  uint8_t position; // The horizontal coordinate of the beginning of the brick
  uint8_t width;    // The width of the brick in pixels
} Brick;

typedef struct {
  GPIO_TypeDef *port;
  uint16_t pin;
  GPIO_PinState active_state;
} GameButtonInfo;

typedef enum {
  BTN_NONE = 0,
  BTN_KEY,
  BTN_JOY,
  BTN_ANY,
} GameButton;

// Public functions

void setup(void);
void loop(void);

/**
 * @brief TIM6 Global Interrupt Service Routine
 *
 * This function runs every `htim6.Init.Period / 10` milliseconds.
 * @see MX_TIM6_Init()
 */
void on_tim6(void);

/**
 * @brief EXTI line[15:10] Interrupts Service Routine
 *
 * Pressing the `User_Button` (Labeled "Key" on the PCB)
 * or the `JOY_SEL` (Pressing down the joystick)
 * triggers EXTI10, in which triggers this function.
 */
void on_button_press(void);

// Game state handlers

void handle_init_state(void);
void handle_playing_state(void);
void handle_paused_state(void);

// Game utilities

int brick_place(void);
void brick_spawning(void);

/**
 * Display the bricks on the LCD page 3 through 7.
 */
void display_bricks(void);

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
 * Detect for button presses by resetting a global variable, and wait for changes from the ISR.
 * @param target The button to wait for. Use BTN_ANY to detect multiple buttons.
 * @param task Task to do while waiting for button press.
 * @return The button pressed.
 */
GameButton await_button(GameButton target, void (*task)());

// Globals

extern UART_HandleTypeDef huart1;
extern ADC_HandleTypeDef hadc1;
extern TIM_HandleTypeDef htim6;

#endif // !APP_H
