#include "uart_stdio.h"

extern UART_HandleTypeDef huart1;

PUTCHAR_PROTOTYPE {
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xffff);
  return ch;
}

int uart_scanf(const char *format, ...) {
  char ch, buf[SCANF_MAX];
  size_t idx = 0;

  do {
    // Wait up to a minute for each character
    HAL_StatusTypeDef status = HAL_UART_Receive(&huart1, (uint8_t *)&ch, 1, 60000);
    if (status != HAL_OK) return EOF;

    buf[idx] = ch;
    putchar(ch);
  } while (ch != '\r' && idx++ < SCANF_MAX);
  puts(CRLF);

  va_list args;
  va_start(args, format);
  int matched = vsscanf(buf, format, args);
  va_end(args);

  return matched;
}
