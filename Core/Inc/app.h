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
  GameState state;
  uint8_t difficulty;
  size_t score;
} GameSession;

// Public functions

void setup(void);
void loop(void);
int brick_place(void);
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

// Private functions

uint32_t read_adc(void);
void handle_init_state(void);
void handle_playing_state(void);
void handle_paused_state(void);

/**
 * @brief Print a line centered in the console.
 * @param s The string to print.
 * @param width The width of the console.
 * @return Whatever puts(s) returns.
 */
int centered_puts(const char *s, uint8_t width);

// Globals

extern UART_HandleTypeDef huart1;
extern ADC_HandleTypeDef hadc1;
extern TIM_HandleTypeDef htim6;

#endif // !APP_H
