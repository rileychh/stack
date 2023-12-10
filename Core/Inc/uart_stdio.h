// Hours wasted here:
// https://www.reddit.com/r/embedded/comments/10smuyq/stm32cubemx_freezes_with_printf/

#ifndef UART_STDIO_H
#define UART_STDIO_H
#include "stm32f1xx_hal.h"

#include <stdarg.h>
#include <stdio.h>

#define SCANF_MAX 128

#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#define GETCHAR_PROTOTYPE int __io_getchar(FILE *f)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#define GETCHAR_PROTOTYPE int fgetc(FILE *f)
#endif // !__GNUC__

PUTCHAR_PROTOTYPE;
GETCHAR_PROTOTYPE;

#endif // !UART_STDIO_H
