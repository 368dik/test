#include "bsp_i2c.h"

#define BSP_I2C1_SCL_PORT GPIOB
#define BSP_I2C1_SCL_PIN  GPIO_Pin_6
#define BSP_I2C1_SDA_PORT GPIOB
#define BSP_I2C1_SDA_PIN  GPIO_Pin_7

static void BSP_I2C1_WriteSCL(uint8_t level)
{
	GPIO_WriteBit(BSP_I2C1_SCL_PORT, BSP_I2C1_SCL_PIN, (BitAction)level);
}

static void BSP_I2C1_WriteSDA(uint8_t level)
{
	GPIO_WriteBit(BSP_I2C1_SDA_PORT, BSP_I2C1_SDA_PIN, (BitAction)level);
}

void BSP_I2C1_SoftInit(void)
{
	GPIO_InitTypeDef gpioInitStruct;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	gpioInitStruct.GPIO_Mode = GPIO_Mode_Out_OD;
	gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;

	gpioInitStruct.GPIO_Pin = BSP_I2C1_SCL_PIN;
	GPIO_Init(BSP_I2C1_SCL_PORT, &gpioInitStruct);

	gpioInitStruct.GPIO_Pin = BSP_I2C1_SDA_PIN;
	GPIO_Init(BSP_I2C1_SDA_PORT, &gpioInitStruct);

	BSP_I2C1_WriteSCL(1);
	BSP_I2C1_WriteSDA(1);
}

void BSP_I2C1_Start(void)
{
	BSP_I2C1_WriteSDA(1);
	BSP_I2C1_WriteSCL(1);
	BSP_I2C1_WriteSDA(0);
	BSP_I2C1_WriteSCL(0);
}

void BSP_I2C1_Stop(void)
{
	BSP_I2C1_WriteSDA(0);
	BSP_I2C1_WriteSCL(1);
	BSP_I2C1_WriteSDA(1);
}

void BSP_I2C1_WriteByte(uint8_t byte)
{
	uint8_t bitIndex;
	for (bitIndex = 0; bitIndex < 8; bitIndex++)
	{
		BSP_I2C1_WriteSDA(byte & (0x80 >> bitIndex));
		BSP_I2C1_WriteSCL(1);
		BSP_I2C1_WriteSCL(0);
	}
	BSP_I2C1_WriteSCL(1);
	BSP_I2C1_WriteSCL(0);
}
