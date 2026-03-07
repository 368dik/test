#include "bsp_spi.h"
#include "Delay.h"

/* 软件SPI1引脚映射（用于W25Q64） */
#define BSP_SPI1_PORT GPIOB
#define BSP_SPI1_CS   GPIO_Pin_12
#define BSP_SPI1_SCK  GPIO_Pin_13
#define BSP_SPI1_MISO GPIO_Pin_14
#define BSP_SPI1_MOSI GPIO_Pin_15

/* 拉低时钟 */
static void BSP_SPI1_SCK_Low(void)
{
	GPIO_ResetBits(BSP_SPI1_PORT, BSP_SPI1_SCK);
}

/* 拉高时钟 */
static void BSP_SPI1_SCK_High(void)
{
	GPIO_SetBits(BSP_SPI1_PORT, BSP_SPI1_SCK);
}

/* 写MOSI */
static void BSP_SPI1_MOSI_Write(uint8_t level)
{
	GPIO_WriteBit(BSP_SPI1_PORT, BSP_SPI1_MOSI, (BitAction)level);
}

/* 读MISO */
static uint8_t BSP_SPI1_MISO_Read(void)
{
	return (uint8_t)GPIO_ReadInputDataBit(BSP_SPI1_PORT, BSP_SPI1_MISO);
}

/* 初始化软件SPI GPIO */
void BSP_SPI1_SoftInit(void)
{
	GPIO_InitTypeDef gpioInitStruct;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	gpioInitStruct.GPIO_Pin = BSP_SPI1_CS | BSP_SPI1_SCK | BSP_SPI1_MOSI;
	gpioInitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(BSP_SPI1_PORT, &gpioInitStruct);

	gpioInitStruct.GPIO_Pin = BSP_SPI1_MISO;
	gpioInitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(BSP_SPI1_PORT, &gpioInitStruct);

	GPIO_SetBits(BSP_SPI1_PORT, BSP_SPI1_CS);
	BSP_SPI1_SCK_Low();
	BSP_SPI1_MOSI_Write(0);
}

/* SPI Mode0位时序传输 */
uint8_t BSP_SPI1_TransferByte(uint8_t value)
{
	uint8_t bitIndex;
	uint8_t rx = 0;

	for (bitIndex = 0; bitIndex < 8; bitIndex++)
	{
		BSP_SPI1_SCK_Low();
		BSP_SPI1_MOSI_Write((value & 0x80) ? 1 : 0);
		value <<= 1;
		Delay_us(1);

		BSP_SPI1_SCK_High();
		rx <<= 1;
		if (BSP_SPI1_MISO_Read())
		{
			rx |= 1;
		}
		Delay_us(1);
	}

	BSP_SPI1_SCK_Low();
	return rx;
}

/* 使能片选 */
void BSP_SPI1_CS_Enable(void)
{
	GPIO_ResetBits(BSP_SPI1_PORT, BSP_SPI1_CS);
}

/* 关闭片选 */
void BSP_SPI1_CS_Disable(void)
{
	GPIO_SetBits(BSP_SPI1_PORT, BSP_SPI1_CS);
}
