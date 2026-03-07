#include "bsp_uart.h"
#include <stdio.h>

void Serial_Init(void)
{
	BSP_UART1_Init(9600);
}

void Serial_Byte(uint8_t Byte)
{
	BSP_UART1_SendByte(Byte);
}

int fputc(int ch,FILE *f)
{
	Serial_Byte((uint8_t)ch);
	return (int)ch;
}
