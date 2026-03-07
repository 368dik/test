#include "drv_flash.h"
#include "bsp_spi.h"

/* W25Q64常用指令码（见芯片手册） */
#define CMD_WRITE_ENABLE 0x06
#define CMD_READ_STATUS1 0x05
#define CMD_PAGE_PROGRAM 0x02
#define CMD_READ_DATA    0x03
#define CMD_SECTOR_ERASE 0x20

/* 写使能：执行写/擦除前必须先发送 */
static void Flash_WriteEnable(void)
{
	BSP_SPI1_CS_Enable();
	BSP_SPI1_TransferByte(CMD_WRITE_ENABLE);
	BSP_SPI1_CS_Disable();
}

/* 读取状态寄存器1 */
static uint8_t Flash_ReadStatus1(void)
{
	uint8_t status;

	BSP_SPI1_CS_Enable();
	BSP_SPI1_TransferByte(CMD_READ_STATUS1);
	status = BSP_SPI1_TransferByte(0xFF);
	BSP_SPI1_CS_Disable();

	return status;
}

/* 轮询等待忙位清零 */
static void Flash_WaitBusy(void)
{
	while (Flash_ReadStatus1() & 0x01)
	{
	}
}

/* 发送24位地址（A23~A0） */
static void Flash_SendAddress24(uint32_t address)
{
	BSP_SPI1_TransferByte((uint8_t)(address >> 16));
	BSP_SPI1_TransferByte((uint8_t)(address >> 8));
	BSP_SPI1_TransferByte((uint8_t)address);
}

/* 初始化Flash底层接口 */
void Flash_Init(void)
{
	BSP_SPI1_SoftInit();
}

/* 连续读数据 */
void Flash_Read(uint32_t address, uint8_t *buffer, uint16_t length)
{
	uint16_t index;

	BSP_SPI1_CS_Enable();
	BSP_SPI1_TransferByte(CMD_READ_DATA);
	Flash_SendAddress24(address);
	for (index = 0; index < length; index++)
	{
		buffer[index] = BSP_SPI1_TransferByte(0xFF);
	}
	BSP_SPI1_CS_Disable();
}

/* 擦除4KB扇区 */
void Flash_EraseSector4K(uint32_t address)
{
	Flash_WriteEnable();
	BSP_SPI1_CS_Enable();
	BSP_SPI1_TransferByte(CMD_SECTOR_ERASE);
	Flash_SendAddress24(address);
	BSP_SPI1_CS_Disable();
	Flash_WaitBusy();
}

/* 页编程写入，自动处理跨页 */
void Flash_Write(uint32_t address, const uint8_t *buffer, uint16_t length)
{
	uint16_t pageRemain;
	uint16_t chunk;
	uint16_t index;

	while (length > 0)
	{
		pageRemain = (uint16_t)(256 - (address & 0xFF));
		chunk = (length < pageRemain) ? length : pageRemain;

		Flash_WriteEnable();
		BSP_SPI1_CS_Enable();
		BSP_SPI1_TransferByte(CMD_PAGE_PROGRAM);
		Flash_SendAddress24(address);
		for (index = 0; index < chunk; index++)
		{
			BSP_SPI1_TransferByte(buffer[index]);
		}
		BSP_SPI1_CS_Disable();
		Flash_WaitBusy();

		address += chunk;
		buffer += chunk;
		length -= chunk;
	}
}
