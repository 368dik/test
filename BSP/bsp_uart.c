#if !defined(STM32F10X_LD) && !defined(STM32F10X_LD_VL) && !defined(STM32F10X_MD) && !defined(STM32F10X_MD_VL) && !defined(STM32F10X_HD) && !defined(STM32F10X_HD_VL) && !defined(STM32F10X_XL) && !defined(STM32F10X_CL)
#define STM32F10X_MD
#endif

#ifndef USE_STDPERIPH_DRIVER
#define USE_STDPERIPH_DRIVER
#endif

#include "bsp_uart.h"

static volatile uint8_t uart1RxData;
static volatile uint8_t uart1RxReady;

void BSP_UART1_Init(uint32_t baudrate)
{
	GPIO_InitTypeDef gpioInitStruct;
	USART_InitTypeDef usartInitStruct;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);

	gpioInitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	gpioInitStruct.GPIO_Pin = GPIO_Pin_9;
	gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpioInitStruct);

	gpioInitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gpioInitStruct.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOA, &gpioInitStruct);

	usartInitStruct.USART_BaudRate = baudrate;
	usartInitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	usartInitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	usartInitStruct.USART_Parity = USART_Parity_No;
	usartInitStruct.USART_StopBits = USART_StopBits_1;
	usartInitStruct.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1, &usartInitStruct);

	USART_Cmd(USART1, ENABLE);
}

void BSP_UART1_SendByte(uint8_t byte)
{
	USART_SendData(USART1, byte);
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
	{
	}
}

void BSP_UART1_SendBuffer(const uint8_t *buffer, uint16_t length)
{
	uint16_t index;
	for (index = 0; index < length; index++)
	{
		BSP_UART1_SendByte(buffer[index]);
	}
}

void BSP_UART1_EnableRxIRQ(void)
{
	NVIC_InitTypeDef nvicInitStruct;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	nvicInitStruct.NVIC_IRQChannel = 37;
	nvicInitStruct.NVIC_IRQChannelCmd = ENABLE;
	nvicInitStruct.NVIC_IRQChannelPreemptionPriority = 2;
	nvicInitStruct.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&nvicInitStruct);

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
}

uint8_t BSP_UART1_TryReadByte(uint8_t *outByte)
{
	if (uart1RxReady == 0)
	{
		return 0;
	}

	*outByte = uart1RxData;
	uart1RxReady = 0;
	return 1;
}

void BSP_UART1_DMA_TxInit(void)
{
	DMA_InitTypeDef dmaInitStruct;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	DMA_DeInit(DMA1_Channel4);

	dmaInitStruct.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;
	dmaInitStruct.DMA_MemoryBaseAddr = 0;
	dmaInitStruct.DMA_DIR = DMA_DIR_PeripheralDST;
	dmaInitStruct.DMA_BufferSize = 0;
	dmaInitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dmaInitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dmaInitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	dmaInitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	dmaInitStruct.DMA_Mode = DMA_Mode_Normal;
	dmaInitStruct.DMA_Priority = DMA_Priority_Medium;
	dmaInitStruct.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel4, &dmaInitStruct);

	USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
}

void BSP_UART1_DMA_Send(const uint8_t *buffer, uint16_t length)
{
	DMA_Cmd(DMA1_Channel4, DISABLE);
	DMA_ClearFlag(DMA1_FLAG_TC4);

	DMA1_Channel4->CMAR = (uint32_t)buffer;
	DMA1_Channel4->CNDTR = length;

	DMA_Cmd(DMA1_Channel4, ENABLE);
	while (DMA_GetFlagStatus(DMA1_FLAG_TC4) == RESET)
	{
	}
	DMA_Cmd(DMA1_Channel4, DISABLE);
}

void USART1_IRQHandler(void)
{
	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
	{
		uart1RxData = (uint8_t)USART_ReceiveData(USART1);
		uart1RxReady = 1;
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}
}
