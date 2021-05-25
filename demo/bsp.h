#ifndef __BSP_H
#define __BSP_H
#include <stdarg.h>
#include <stdio.h>
#include "stm32l4xx_hal.h"

void MX_GPIO_Init(void);
void Error_Handler(void);
void SystemClock_Config(void);
void MX_USART2_UART_Init(void);
void MX_TIM7_Init(void);
int uart_send(char *buffer, int length);



#define VCP_TX_Pin GPIO_PIN_2
#define VCP_TX_GPIO_Port GPIOA
#define SWDIO_Pin GPIO_PIN_13
#define SWDIO_GPIO_Port GPIOA
#define SWCLK_Pin GPIO_PIN_14
#define SWCLK_GPIO_Port GPIOA
#define VCP_RX_Pin GPIO_PIN_15
#define VCP_RX_GPIO_Port GPIOA
#define LD3_Pin GPIO_PIN_3
#define LD3_GPIO_Port GPIOB

char *myitoa(int num,char *str,int radix) ;
void SystemClock_Config(void);

//void os_printf(char *fmt, ...);
#endif

