#ifndef __BSP_UART_H
#define __BSP_UART_H

#if !defined(STM32F10X_LD) && !defined(STM32F10X_LD_VL) && !defined(STM32F10X_MD) && !defined(STM32F10X_MD_VL) && !defined(STM32F10X_HD) && !defined(STM32F10X_HD_VL) && !defined(STM32F10X_XL) && !defined(STM32F10X_CL)
#define STM32F10X_MD
#endif

#ifndef USE_STDPERIPH_DRIVER
#define USE_STDPERIPH_DRIVER
#endif

#include "stm32f10x.h"

void BSP_UART1_Init(uint32_t baudrate);
void BSP_UART1_SendByte(uint8_t byte);
void BSP_UART1_SendBuffer(const uint8_t *buffer, uint16_t length);

void BSP_UART1_EnableRxIRQ(void);
uint8_t BSP_UART1_TryReadByte(uint8_t *outByte);

void BSP_UART1_DMA_TxInit(void);
void BSP_UART1_DMA_Send(const uint8_t *buffer, uint16_t length);

#endif
