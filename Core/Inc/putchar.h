// Hours wasted here:
// https://www.reddit.com/r/embedded/comments/10smuyq/stm32cubemx_freezes_with_printf/

#ifndef PUTCHAR_H
#define PUTCHAR_H
#include "stm32f1xx_hal.h"

#include <stdio.h>

#ifdef __GNUC__
// With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf set to 'Yes') calls __io_putchar()
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif // !__GNUC__

PUTCHAR_PROTOTYPE {
  extern UART_HandleTypeDef huart1;
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xffff);
  return ch;
}

#endif // !PUTCHAR_H
