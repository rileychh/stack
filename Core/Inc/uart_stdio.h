// Hours wasted here:
// https://www.reddit.com/r/embedded/comments/10smuyq/stm32cubemx_freezes_with_printf/

#ifndef UART_STDIO_H
#define UART_STDIO_H
#include "stm32f1xx_hal.h"

#include <stdarg.h>
#include <stdio.h>

#define SCANF_MAX 128
#define CRLF "\r\n"

#ifdef __GNUC__
// With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf set to 'Yes') calls __io_putchar()
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif // !__GNUC__

PUTCHAR_PROTOTYPE;

int uart_scanf(const char *format, ...);
#define scanf uart_scanf

#endif // !UART_STDIO_H
