/*
 * bsp_uart.c - UART 硬件抽象层实现
 *
 * 基于 MSPM0 DriverLib。SysConfig 已通过生成的 SYSCFG_DL_init()
 * 完成 UART 外设配置（引脚/时钟/波特率/中断使能），本层仅：
 *   - 补上 SysConfig 不管的 NVIC 使能
 *   - 封装 DriverLib 的收发和中断查询 API
 *
 * 移植到其他 MCU 系列时，仅需重写本文件。
 */

#include "bsp_uart.h"

/**
 * @brief  初始化 UART（NVIC 使能）
 * @param  huart  UART 句柄指针
 * @note   SYSCFG_DL_init() 已配置好外设，这里只开 NVIC
 */
void BSP_UART_Init(BSP_UART_Handle *huart) {
    NVIC_EnableIRQ(huart->irqn);
}

/**
 * @brief  阻塞发送一个字节
 */
void BSP_UART_SendByte(BSP_UART_Handle *huart, uint8_t data) {
    DL_UART_transmitDataBlocking(huart->inst, data);
}

/**
 * @brief  阻塞发送字节数组
 */
void BSP_UART_SendBuffer(BSP_UART_Handle *huart, const uint8_t *buf, uint16_t len) {
    uint16_t i;
    for (i = 0; i < len; i++) {
        DL_UART_transmitDataBlocking(huart->inst, buf[i]);
    }
}

/**
 * @brief  从 RX FIFO 读取一个字节（非阻塞）
 * @param  huart  UART 句柄指针
 * @return 接收到的字节
 * @note   调用前需确保 FIFO 中有数据（通常在 ISR 中调用）
 */
uint8_t BSP_UART_ReceiveByte(BSP_UART_Handle *huart) {
    return DL_UART_receiveData(huart->inst);
}

/**
 * @brief  获取当前挂起的中断类型
 * @param  huart  UART 句柄指针
 * @return 中断类型（与 DL_UART_IIDX_RX 等宏比较）
 */
uint32_t BSP_UART_GetPendingIRQ(BSP_UART_Handle *huart) {
    return DL_UART_getPendingInterrupt(huart->inst);
}
