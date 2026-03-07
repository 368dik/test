#ifndef __BSP_SPI_H
#define __BSP_SPI_H

#include "stm32f10x.h"

/* SPI1软件模拟初始化（PB12~PB15） */
void BSP_SPI1_SoftInit(void);
/* 发送1字节并返回同时接收的1字节 */
uint8_t BSP_SPI1_TransferByte(uint8_t value);
/* 拉低片选（开始一次SPI事务） */
void BSP_SPI1_CS_Enable(void);
/* 拉高片选（结束一次SPI事务） */
void BSP_SPI1_CS_Disable(void);

#endif
