#ifndef __DRV_DHT11_H
#define __DRV_DHT11_H

#include "stdint.h"

/* DHT11解析后的温湿度数据 */
typedef struct
{
	uint8_t temperature;
	uint8_t humidity;
} DHT11_Data;

/* 初始化DHT11驱动 */
void DHT11_Init(void);
/* 读取一次DHT11温湿度，成功返回1 */
uint8_t DHT11_Read(DHT11_Data *outData);
/* 获取最近一次读取失败码，0表示无错误 */
uint8_t DHT11_GetLastError(void);

#endif
