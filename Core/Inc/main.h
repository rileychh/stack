/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define A19_Pin GPIO_PIN_3
#define A19_GPIO_Port GPIOE
#define A20_Pin GPIO_PIN_4
#define A20_GPIO_Port GPIOE
#define A21_Pin GPIO_PIN_5
#define A21_GPIO_Port GPIOE
#define A22_Pin GPIO_PIN_6
#define A22_GPIO_Port GPIOE
#define Anti_Tamper_Pin GPIO_PIN_13
#define Anti_Tamper_GPIO_Port GPIOC
#define A0_Pin GPIO_PIN_0
#define A0_GPIO_Port GPIOF
#define A1_Pin GPIO_PIN_1
#define A1_GPIO_Port GPIOF
#define A2_Pin GPIO_PIN_2
#define A2_GPIO_Port GPIOF
#define A3_Pin GPIO_PIN_3
#define A3_GPIO_Port GPIOF
#define A4_Pin GPIO_PIN_4
#define A4_GPIO_Port GPIOF
#define A5_Pin GPIO_PIN_5
#define A5_GPIO_Port GPIOF
#define LED1_Pin GPIO_PIN_6
#define LED1_GPIO_Port GPIOF
#define LED2_Pin GPIO_PIN_7
#define LED2_GPIO_Port GPIOF
#define LED3_Pin GPIO_PIN_8
#define LED3_GPIO_Port GPIOF
#define LED4_Pin GPIO_PIN_9
#define LED4_GPIO_Port GPIOF
#define WAKEUP_Pin GPIO_PIN_0
#define WAKEUP_GPIO_Port GPIOA
#define USART2_RTS_Pin GPIO_PIN_1
#define USART2_RTS_GPIO_Port GPIOA
#define USART2_TX_Pin GPIO_PIN_2
#define USART2_TX_GPIO_Port GPIOA
#define USART2_RX_Pin GPIO_PIN_3
#define USART2_RX_GPIO_Port GPIOA
#define SPI1_SCK_Pin GPIO_PIN_5
#define SPI1_SCK_GPIO_Port GPIOA
#define SPI1_MISO_Pin GPIO_PIN_6
#define SPI1_MISO_GPIO_Port GPIOA
#define SPI1_MOSI_Pin GPIO_PIN_7
#define SPI1_MOSI_GPIO_Port GPIOA
#define Potentiometer_Pin GPIO_PIN_4
#define Potentiometer_GPIO_Port GPIOC
#define SPI1_CS_Pin GPIO_PIN_2
#define SPI1_CS_GPIO_Port GPIOB
#define MicroSDCard_Detect_Pin GPIO_PIN_11
#define MicroSDCard_Detect_GPIO_Port GPIOF
#define A6_Pin GPIO_PIN_12
#define A6_GPIO_Port GPIOF
#define A7_Pin GPIO_PIN_13
#define A7_GPIO_Port GPIOF
#define A8_Pin GPIO_PIN_14
#define A8_GPIO_Port GPIOF
#define A9_Pin GPIO_PIN_15
#define A9_GPIO_Port GPIOF
#define A10_Pin GPIO_PIN_0
#define A10_GPIO_Port GPIOG
#define A11_Pin GPIO_PIN_1
#define A11_GPIO_Port GPIOG
#define D_Pin GPIO_PIN_7
#define D_GPIO_Port GPIOE
#define D5_Pin GPIO_PIN_8
#define D5_GPIO_Port GPIOE
#define D6_Pin GPIO_PIN_9
#define D6_GPIO_Port GPIOE
#define D7_Pin GPIO_PIN_10
#define D7_GPIO_Port GPIOE
#define D8_Pin GPIO_PIN_11
#define D8_GPIO_Port GPIOE
#define D9_Pin GPIO_PIN_12
#define D9_GPIO_Port GPIOE
#define D10_Pin GPIO_PIN_13
#define D10_GPIO_Port GPIOE
#define D11_Pin GPIO_PIN_14
#define D11_GPIO_Port GPIOE
#define D12_Pin GPIO_PIN_15
#define D12_GPIO_Port GPIOE
#define I2S_CMD_Pin GPIO_PIN_12
#define I2S_CMD_GPIO_Port GPIOB
#define I2S_CK_Pin GPIO_PIN_13
#define I2S_CK_GPIO_Port GPIOB
#define USB_Disconnect_Pin GPIO_PIN_14
#define USB_Disconnect_GPIO_Port GPIOB
#define I2S_DIN_Pin GPIO_PIN_15
#define I2S_DIN_GPIO_Port GPIOB
#define D13_Pin GPIO_PIN_8
#define D13_GPIO_Port GPIOD
#define D14_Pin GPIO_PIN_9
#define D14_GPIO_Port GPIOD
#define D15_Pin GPIO_PIN_10
#define D15_GPIO_Port GPIOD
#define A16_Pin GPIO_PIN_11
#define A16_GPIO_Port GPIOD
#define A17_Pin GPIO_PIN_12
#define A17_GPIO_Port GPIOD
#define A18_Pin GPIO_PIN_13
#define A18_GPIO_Port GPIOD
#define D0_Pin GPIO_PIN_14
#define D0_GPIO_Port GPIOD
#define D1_Pin GPIO_PIN_15
#define D1_GPIO_Port GPIOD
#define A12_Pin GPIO_PIN_2
#define A12_GPIO_Port GPIOG
#define A13_Pin GPIO_PIN_3
#define A13_GPIO_Port GPIOG
#define A14_Pin GPIO_PIN_4
#define A14_GPIO_Port GPIOG
#define A15_Pin GPIO_PIN_5
#define A15_GPIO_Port GPIOG
#define JOY_SEL_Pin GPIO_PIN_7
#define JOY_SEL_GPIO_Port GPIOG
#define User_Button_Pin GPIO_PIN_8
#define User_Button_GPIO_Port GPIOG
#define I2S_MCK_Pin GPIO_PIN_6
#define I2S_MCK_GPIO_Port GPIOC
#define MicroSDCard_D0_Pin GPIO_PIN_8
#define MicroSDCard_D0_GPIO_Port GPIOC
#define MicroSDCard_D1_Pin GPIO_PIN_9
#define MicroSDCard_D1_GPIO_Port GPIOC
#define USART1_TX_Pin GPIO_PIN_9
#define USART1_TX_GPIO_Port GPIOA
#define USART1_RX_Pin GPIO_PIN_10
#define USART1_RX_GPIO_Port GPIOA
#define USB_DM_Pin GPIO_PIN_11
#define USB_DM_GPIO_Port GPIOA
#define TMS_SWDIO_Pin GPIO_PIN_13
#define TMS_SWDIO_GPIO_Port GPIOA
#define TCK_SWCLK_Pin GPIO_PIN_14
#define TCK_SWCLK_GPIO_Port GPIOA
#define TDI_Pin GPIO_PIN_15
#define TDI_GPIO_Port GPIOA
#define MicroSDCard_D2_Pin GPIO_PIN_10
#define MicroSDCard_D2_GPIO_Port GPIOC
#define MicroSDCard_D3_Pin GPIO_PIN_11
#define MicroSDCard_D3_GPIO_Port GPIOC
#define MicroSDCard_CLK_Pin GPIO_PIN_12
#define MicroSDCard_CLK_GPIO_Port GPIOC
#define D2_Pin GPIO_PIN_0
#define D2_GPIO_Port GPIOD
#define D3_Pin GPIO_PIN_1
#define D3_GPIO_Port GPIOD
#define MicroSDCard_CMD_Pin GPIO_PIN_2
#define MicroSDCard_CMD_GPIO_Port GPIOD
#define JOY_DOWN_Pin GPIO_PIN_3
#define JOY_DOWN_GPIO_Port GPIOD
#define FSMC_NOE_Pin GPIO_PIN_4
#define FSMC_NOE_GPIO_Port GPIOD
#define FSMC_NWE_Pin GPIO_PIN_5
#define FSMC_NWE_GPIO_Port GPIOD
#define FSMC_NWAIT_Pin GPIO_PIN_6
#define FSMC_NWAIT_GPIO_Port GPIOD
#define FSMC_NCE2_Pin GPIO_PIN_7
#define FSMC_NCE2_GPIO_Port GPIOD
#define FSMC_NE2_Pin GPIO_PIN_9
#define FSMC_NE2_GPIO_Port GPIOG
#define FSMC_NE3_Pin GPIO_PIN_10
#define FSMC_NE3_GPIO_Port GPIOG
#define Audio_PDN_Pin GPIO_PIN_11
#define Audio_PDN_GPIO_Port GPIOG
#define FSMC_NE4_Pin GPIO_PIN_12
#define FSMC_NE4_GPIO_Port GPIOG
#define JOY_RIGHT_Pin GPIO_PIN_13
#define JOY_RIGHT_GPIO_Port GPIOG
#define JOY_LEFT_Pin GPIO_PIN_14
#define JOY_LEFT_GPIO_Port GPIOG
#define JOY_UP_Pin GPIO_PIN_15
#define JOY_UP_GPIO_Port GPIOG
#define TDO_SWO_Pin GPIO_PIN_3
#define TDO_SWO_GPIO_Port GPIOB
#define TRST_Pin GPIO_PIN_4
#define TRST_GPIO_Port GPIOB
#define TemperatureSensor_INT_Pin GPIO_PIN_5
#define TemperatureSensor_INT_GPIO_Port GPIOB
#define I2C_SCK_Pin GPIO_PIN_6
#define I2C_SCK_GPIO_Port GPIOB
#define I2C_SDA_Pin GPIO_PIN_7
#define I2C_SDA_GPIO_Port GPIOB
#define CAN_RX_Pin GPIO_PIN_8
#define CAN_RX_GPIO_Port GPIOB
#define CAN_TX_Pin GPIO_PIN_9
#define CAN_TX_GPIO_Port GPIOB
#define FMC_NBL0_Pin GPIO_PIN_0
#define FMC_NBL0_GPIO_Port GPIOE
#define FMC_NBL1_Pin GPIO_PIN_1
#define FMC_NBL1_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
