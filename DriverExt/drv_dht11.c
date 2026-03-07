#include "drv_dht11.h"
#include "bsp_dht11.h"
#include "stm32f10x.h"

/* DHT11读数参数 */
#define DHT11_MAX_RETRY_COUNT 3
#define DHT11_RESP_TIMEOUT_US 200
#define DHT11_BIT_TIMEOUT_US  120
#define DHT11_SAMPLE_DELAY_US 40

static uint8_t g_dht11LastError;

/* 在限定时间内等待引脚达到目标电平 */
static uint8_t DHT11_WaitLevel(uint8_t level, uint16_t timeoutUs)
{
	while (timeoutUs > 0)
	{
		if (BSP_DHT11_ReadLevel() == level)
		{
			return 1;
		}
		BSP_DHT11_DelayUs(1);
		timeoutUs--;
	}

	return 0;
}

/* 初始化DHT11驱动状态 */
void DHT11_Init(void)
{
	BSP_DHT11_Init();
	g_dht11LastError = 0;
}

/* 执行一次完整的DHT11读数过程 */
static uint8_t DHT11_ReadOnce(DHT11_Data *outData)
{
	uint8_t bytes[5];
	uint8_t index;
	uint8_t bitIndex;
	uint8_t bit;
	uint8_t success = 1;
	g_dht11LastError = 0;

	/* DHT11时序严格，读流程期间临时关闭中断 */
	__disable_irq();

	BSP_DHT11_PinModeOutput();
	BSP_DHT11_WriteLevel(0);
	BSP_DHT11_DelayMs(20);
	BSP_DHT11_WriteLevel(1);
	BSP_DHT11_DelayUs(30);

	BSP_DHT11_PinModeInput();

	if (!DHT11_WaitLevel(0, DHT11_RESP_TIMEOUT_US))
	{
		g_dht11LastError = 1;
		success = 0;
		goto END;
	}
	if (!DHT11_WaitLevel(1, DHT11_RESP_TIMEOUT_US))
	{
		g_dht11LastError = 2;
		success = 0;
		goto END;
	}
	if (!DHT11_WaitLevel(0, DHT11_RESP_TIMEOUT_US))
	{
		g_dht11LastError = 3;
		success = 0;
		goto END;
	}

	for (index = 0; index < 5; index++)
	{
		bytes[index] = 0;
		for (bitIndex = 0; bitIndex < 8; bitIndex++)
		{
			if (!DHT11_WaitLevel(1, DHT11_BIT_TIMEOUT_US))
			{
				g_dht11LastError = 4;
				success = 0;
				goto END;
			}

			BSP_DHT11_DelayUs(DHT11_SAMPLE_DELAY_US);
			bit = BSP_DHT11_ReadLevel();

			if (!DHT11_WaitLevel(0, DHT11_BIT_TIMEOUT_US))
			{
				g_dht11LastError = 5;
				success = 0;
				goto END;
			}

			bytes[index] <<= 1;
			bytes[index] |= bit;
		}
	}

	if ((uint8_t)(bytes[0] + bytes[1] + bytes[2] + bytes[3]) != bytes[4])
	{
		g_dht11LastError = 6;
		success = 0;
		goto END;
	}

	outData->humidity = bytes[0];
	outData->temperature = bytes[2];

END:
	BSP_DHT11_PinModeOutput();
	BSP_DHT11_WriteLevel(1);
	__enable_irq();
	return success;
}

uint8_t DHT11_Read(DHT11_Data *outData)
{
	uint8_t retry;

	/* 参数检查 */
	if (outData == 0)
	{
		return 0;
	}

	for (retry = 0; retry < DHT11_MAX_RETRY_COUNT; retry++)
	{
		if (DHT11_ReadOnce(outData))
		{
			return 1;
		}

		BSP_DHT11_DelayMs(30);
	}

	return 0;
}

/* 获取最近一次读数失败码 */
uint8_t DHT11_GetLastError(void)
{
	return g_dht11LastError;
}
