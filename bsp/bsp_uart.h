/*
 * bsp_uart.h - UART 硬件抽象层 (BSP)
 *
 * 封装 MSPM0 DriverLib 的 UART 操作，向上层提供统一接口。
 * 外设配置（引脚/波特率/中断）在 .syscfg 图形界面中完成，
 * BSP_Init 仅补上 SysConfig 不做的那一步：NVIC 使能。
 *
 * 移植到其他 MCU 系列时，只需重写 bsp_uart.c，本头文件保持不变。
 */

#ifndef __BSP_UART_H
#define __BSP_UART_H

#include "ti_msp_dl_config.h"

/* UART 句柄 */
typedef struct {
    UART_Regs *inst;  /* UART 外设基址，如 UART0              */
    IRQn_Type  irqn;  /* NVIC 中断号，如 UART_0_INST_INT_IRQN */
} BSP_UART_Handle;

void BSP_UART_Init(BSP_UART_Handle *huart);
void BSP_UART_SendByte(BSP_UART_Handle *huart, uint8_t data);
void BSP_UART_SendBuffer(BSP_UART_Handle *huart, const uint8_t *buf, uint16_t len);
uint8_t BSP_UART_ReceiveByte(BSP_UART_Handle *huart);
uint32_t BSP_UART_GetPendingIRQ(BSP_UART_Handle *huart);

#endif
