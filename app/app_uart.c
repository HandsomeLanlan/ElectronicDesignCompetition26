/*
 * app_uart.c - UART 应用层实现
 *
 * 提供环形缓冲接收、格式化打印、中断服务函数。
 * 本文件不依赖具体 MCU 寄存器，只通过 BSP 层接口操作硬件。
 *
 * 移植注意事项：
 *   - 如果使用 UART1/UART2 等非 UART0 实例，修改下方 ISR 函数名
 *     UART0_IRQHandler → UART1_IRQHandler（函数名必须匹配启动文件的向量表）
 *   - RX_BUF_SIZE 可根据需要调整，必须为 2 的幂
 */

#include "app_uart.h"
#include <stdarg.h>
#include <stdio.h>

/*
 * 环形接收缓冲区，容量必须为 2 的幂（方便用位与代替取模）。
 * 故意闲置一个槽位：writeIdx 追上 readIdx 代表"满"，相等代表"空"。
 * 实际可用容量 = RX_BUF_SIZE - 1
 */
#define RX_BUF_SIZE  256
#define RX_BUF_MASK  (RX_BUF_SIZE - 1)

/* 当前绑定的 UART 句柄，由 APP_UART_Init 设置 */
static BSP_UART_Handle *g_huart;

/* 环形缓冲区及读写指针 */
static volatile uint8_t  rx_buf[RX_BUF_SIZE];
static volatile uint16_t writeIdx;  /* ISR 写入位置 */
static volatile uint16_t readIdx;  /* 用户读取位置 */

/**
 * @brief  初始化应用层 UART
 * @param  huart  BSP 层句柄指针
 * @note   保存句柄、清零环形缓冲指针、调用 BSP 硬件初始化
 */
void APP_UART_Init(BSP_UART_Handle *huart) {
    g_huart = huart;
    writeIdx = 0;
    readIdx  = 0;
    BSP_UART_Init(huart);
}

/*
 * UART0 中断服务函数
 *
 * 函数名必须与启动文件向量表中的名称一致：
 *   UART0 → UART0_IRQHandler
 *   UART1 → UART1_IRQHandler
 *   UART2 → UART2_IRQHandler
 *
 * 更换 UART 实例时，修改函数名和 APP_UART_Init 传入的句柄即可。
 */
void UART0_IRQHandler(void) {
    uint16_t nextWriteIdx;

    switch (BSP_UART_GetPendingIRQ(g_huart)) {
        case DL_UART_IIDX_RX: {
            uint8_t data = BSP_UART_ReceiveByte(g_huart);
            nextWriteIdx = (writeIdx + 1) & RX_BUF_MASK;
            if (nextWriteIdx != readIdx) {
                /* 缓冲未满：写入并推进指针 */
                rx_buf[writeIdx] = data;
                writeIdx = nextWriteIdx;
            }
            /* 缓冲满：丢弃该字节 */
            break;
        }
        default:
            break;
    }
}

/**
 * @brief  发送字符串（阻塞）
 * @param  str  以 '\0' 结尾的字符串
 * @note   逐字节调用 BSP 阻塞发送，字符串较长时会阻塞较长时间
 */
void APP_UART_SendString(const char *str) {
    while (*str) {
        BSP_UART_SendByte(g_huart, (uint8_t)*str ++ );
    }
}

/**
 * @brief  格式化打印（类似 printf）
 * @param  fmt  格式化字符串
 * @param  ...  可变参数
 * @note   内部使用 256 字节栈缓冲区，单次输出不要超过此长度
 */
void APP_UART_Printf(const char *fmt, ...) {
    static char buf[256];
    va_list args;
    int len;

    va_start(args, fmt);
    len = vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    if (len > 0) {
        BSP_UART_SendBuffer(g_huart, (const uint8_t *)buf, len);
    }
}

/**
 * @brief  查询环形缓冲中可读字节数
 * @return 可读字节数
 */
uint16_t APP_UART_Available(void) {
    return (writeIdx - readIdx) & RX_BUF_MASK;
}

/**
 * @brief  从环形缓冲读取一个字节
 * @return 读取到的字节
 * @note   调用前需用 APP_UART_Available 确认有数据
 */
uint8_t APP_UART_Read(void) {
    uint8_t data = rx_buf[readIdx];
    readIdx = (readIdx + 1) & RX_BUF_MASK;
    return data;
}
