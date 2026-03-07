#ifndef __BSP_DHT11_H
#define __BSP_DHT11_H

#include "stm32f10x.h"

/*
 * DHT11 BSP配置：
 * 1 表示传感器模块带外部上拉，GPIO采用开漏输出+浮空输入。
 * 0 表示无外部上拉，GPIO采用推挽输出+内部上拉输入。
 */
#define BSP_DHT11_USE_EXTERNAL_PULLUP 1
/* 上电后等待模块稳定时间（ms） */
#define BSP_DHT11_POWER_ON_DELAY_MS   1500

/* 初始化DHT11底层GPIO与默认空闲电平 */
void BSP_DHT11_Init(void);
/* 将DHT11数据引脚切换为输出模式 */
void BSP_DHT11_PinModeOutput(void);
/* 将DHT11数据引脚切换为输入模式 */
void BSP_DHT11_PinModeInput(void);
/* 写DHT11数据引脚电平：1高/0低 */
void BSP_DHT11_WriteLevel(uint8_t level);
/* 读取DHT11数据引脚当前电平：1高/0低 */
uint8_t BSP_DHT11_ReadLevel(void);
/* 微秒级延时（DHT11时序使用） */
void BSP_DHT11_DelayUs(uint32_t us);
/* 毫秒级延时（起始信号和重试间隔使用） */
void BSP_DHT11_DelayMs(uint32_t ms);

#endif
