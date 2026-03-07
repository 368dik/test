#ifndef __BSP_I2C_H
#define __BSP_I2C_H

#include "stm32f10x.h"

void BSP_I2C1_SoftInit(void);
void BSP_I2C1_Start(void);
void BSP_I2C1_Stop(void);
void BSP_I2C1_WriteByte(uint8_t byte);

#endif
