#include "bsp_dht11.h"
#include "Delay.h"

/* DHT11数据引脚：PA8 */
#define BSP_DHT11_PORT GPIOA
#define BSP_DHT11_PIN  GPIO_Pin_8

/* 配置DHT11引脚为输出模式 */
void BSP_DHT11_PinModeOutput(void)
{
    GPIO_InitTypeDef gpioInitStruct;

    gpioInitStruct.GPIO_Pin = BSP_DHT11_PIN;
#if BSP_DHT11_USE_EXTERNAL_PULLUP
    gpioInitStruct.GPIO_Mode = GPIO_Mode_Out_OD;
#else
    gpioInitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
#endif
    gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(BSP_DHT11_PORT, &gpioInitStruct);
}

/* 配置DHT11引脚为输入模式 */
void BSP_DHT11_PinModeInput(void)
{
    GPIO_InitTypeDef gpioInitStruct;

    gpioInitStruct.GPIO_Pin = BSP_DHT11_PIN;
#if BSP_DHT11_USE_EXTERNAL_PULLUP
    gpioInitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
#else
    gpioInitStruct.GPIO_Mode = GPIO_Mode_IPU;
#endif
    gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(BSP_DHT11_PORT, &gpioInitStruct);
}

/* 输出数据引脚电平 */
void BSP_DHT11_WriteLevel(uint8_t level)
{
    GPIO_WriteBit(BSP_DHT11_PORT, BSP_DHT11_PIN, (BitAction)level);
}

/* 读取数据引脚电平 */
uint8_t BSP_DHT11_ReadLevel(void)
{
    return (uint8_t)GPIO_ReadInputDataBit(BSP_DHT11_PORT, BSP_DHT11_PIN);
}

/* DHT11微秒延时封装 */
void BSP_DHT11_DelayUs(uint32_t us)
{
    Delay_us(us);
}

/* DHT11毫秒延时封装 */
void BSP_DHT11_DelayMs(uint32_t ms)
{
    Delay_ms(ms);
}

/* 初始化GPIO并将总线释放为高电平 */
void BSP_DHT11_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    BSP_DHT11_PinModeOutput();
    BSP_DHT11_WriteLevel(1);
    BSP_DHT11_DelayMs(BSP_DHT11_POWER_ON_DELAY_MS);
}
