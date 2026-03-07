#ifndef __DRV_FLASH_H
#define __DRV_FLASH_H

#include "stm32f10x.h"

/* 初始化W25Q64底层SPI接口 */
void Flash_Init(void);
/* 从Flash指定地址读取指定长度数据 */
void Flash_Read(uint32_t address, uint8_t *buffer, uint16_t length);
/* 向Flash指定地址写入指定长度数据（内部按页拆分） */
void Flash_Write(uint32_t address, const uint8_t *buffer, uint16_t length);
/* 擦除包含该地址的4KB扇区 */
void Flash_EraseSector4K(uint32_t address);

#endif
