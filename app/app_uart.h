/*
 * app_uart.h - UART 应用层
 *
 * 在 BSP 层之上提供环形缓冲、格式化打印等功能。
 * 本层不依赖任何硬件寄存器，移植时无需修改。
 */

#ifndef __APP_UART_H
#define __APP_UART_H

#include "bsp/bsp_uart.h"

void APP_UART_Init(BSP_UART_Handle *huart);
void APP_UART_SendString(const char *str);
void APP_UART_Printf(const char *fmt, ...);
uint16_t APP_UART_Available(void);
uint8_t APP_UART_Read(void);

#endif
