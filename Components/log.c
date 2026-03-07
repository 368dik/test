#include "log.h"
#include "bsp_uart.h"

static void Log_SendDec(uint32_t value)
{
    char digits[10];
    uint8_t count = 0;

    if (value == 0)
    {
        BSP_UART1_SendByte('0');
        return;
    }

    while (value > 0 && count < sizeof(digits))
    {
        digits[count++] = (char)('0' + (value % 10));
        value /= 10;
    }

    while (count > 0)
    {
        count--;
        BSP_UART1_SendByte((uint8_t)digits[count]);
    }
}

void Log_Init(void)
{
    BSP_UART1_Init(115200);
    BSP_UART1_EnableRxIRQ();
}

void Log_Write(const char *text)
{
    while (*text != '\0')
    {
        BSP_UART1_SendByte((uint8_t)(*text));
        text++;
    }
}

void Log_WriteLine(const char *text)
{
    Log_Write(text);
    BSP_UART1_SendByte('\r');
    BSP_UART1_SendByte('\n');
}

void Log_WriteU32(const char *prefix, uint32_t value)
{
    Log_Write(prefix);
    Log_SendDec(value);
    BSP_UART1_SendByte('\r');
    BSP_UART1_SendByte('\n');
}
