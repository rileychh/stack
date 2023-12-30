#ifndef __LCD_H
#define __LCD_H

#include "stm32f1xx.h"

#define Set_Start_Line_X 0x40
#define Set_Page_Addr_X 0xb0
#define Set_ColH_Addr_X 0x10
#define Set_ColL_Addr_X 0x0

#define LCD_Reset 0xe2
#define Display_Off 0xae
#define Display_On 0xaf
#define Set_ADC_Normal 0xa0
#define Set_ADC_Reverse 0xa1
#define Set_LCD_Bias_7 0xa3
#define Set_LCD_Bias_9 0xa2
#define RMW_Mode_En 0xe0
#define RMW_Mode_Dis 0xee
#define COM_Scan_Dir_Normal 0xc0
#define COM_Scan_Dir_Reverse 0xc8

#define Set_Resistor_Ratio_X 0x20
#define Set_Ref_Vol_Mode 0x81
#define Set_Ref_Vol_Reg 0x20

#define Display_Normal 0xa6
#define Display_Reverse 0xa7
#define Display_All_On 0xa5
#define Display_All_Normal 0xa4

// The 128x64 LCD module is using ST7565
typedef __IO struct {
  uint8_t cmd;  // A0=0
  uint8_t data; // A0=1
} lcd_t;
#define LCD ((lcd_t *)FSMC_BANK1_4)

void lcd_send_command(uint8_t cmd);
void lcd_send_data(uint8_t data);

void LCD_Init(void);
void LCD_Clear(void);

void LCD_draw_fb(unsigned char *fb);
void LCD_DrawChar(unsigned char Xpos, unsigned char Ypos, unsigned char offset);
int LCD_DrawString(unsigned char Xpos, unsigned char Ypos, char *c, unsigned char length);

void LCD_PowerOn(void);
void LCD_DisplayOn(void);
void LCD_DisplayOff(void);

void delay(volatile unsigned long length);
#define command_delay() delay(0x8)
#define reset_delay() delay(0xff)
#define power_delay() delay(0x4ffff)

void LCD_Reset_Cursor(void);
void LCD_Clr_Cursor(void);
void LCD_Set_Cursor(signed char x);

#endif /* __LCD_H */
