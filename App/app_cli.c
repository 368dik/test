#include "app_cli.h"
#include "ring_buffer.h"
#include "log.h"
#include "OLED.h"
#include "storage.h"

#define APP_CLI_RX_BUFFER_SIZE 128
#define APP_CLI_LINE_MAX_LEN   48

/* CLI接收缓存与当前命令行 */
static RingBuffer g_cliRxRb;
static uint8_t g_cliRxMem[APP_CLI_RX_BUFFER_SIZE];
static char g_lineBuf[APP_CLI_LINE_MAX_LEN];
static uint8_t g_lineLen;

/* 简单字符串相等比较 */
static uint8_t AppCli_StrEq(const char *a, const char *b)
{
    while (*a != '\0' && *b != '\0')
    {
        if (*a != *b)
        {
            return 0;
        }
        a++;
        b++;
    }

    return (*a == '\0' && *b == '\0');
}

/* 执行一条完整CLI命令并记录到Storage */
static void AppCli_ExecLine(void)
{
    StorageRecord record;

    g_lineBuf[g_lineLen] = '\0';

    if (AppCli_StrEq(g_lineBuf, "help"))
    {
        Log_WriteLine("cmd: help|ping|stat|clear");
    }
    else if (AppCli_StrEq(g_lineBuf, "ping"))
    {
        Log_WriteLine("pong");
    }
    else if (AppCli_StrEq(g_lineBuf, "stat"))
    {
        Log_WriteU32("records=", Storage_GetCount());
    }
    else if (AppCli_StrEq(g_lineBuf, "clear"))
    {
        OLED_Clear();
        Log_WriteLine("oled cleared");
    }
    else
    {
        Log_WriteLine("unknown cmd");
    }

    record.timestampMs = 0;
    record.type = 1;
    record.length = g_lineLen;
    {
        uint8_t i;
        for (i = 0; i < sizeof(record.payload); i++)
        {
            if (i < g_lineLen)
            {
                record.payload[i] = (uint8_t)g_lineBuf[i];
            }
            else
            {
                record.payload[i] = 0;
            }
        }
    }
    Storage_Append(&record);

    g_lineLen = 0;
}

/* CLI模块初始化 */
void AppCli_Init(void)
{
    RingBuffer_Init(&g_cliRxRb, g_cliRxMem, APP_CLI_RX_BUFFER_SIZE);
    g_lineLen = 0;
}

/* 接收单字节输入 */
void AppCli_OnRxByte(uint8_t byte)
{
    RingBuffer_Push(&g_cliRxRb, byte);
}

/* 处理接收缓冲并按行执行命令 */
void AppCli_Process(void)
{
    uint8_t byte;

    while (RingBuffer_Pop(&g_cliRxRb, &byte))
    {
        if (byte == '\r' || byte == '\n')
        {
            if (g_lineLen > 0)
            {
                AppCli_ExecLine();
            }
            continue;
        }

        if (g_lineLen < (APP_CLI_LINE_MAX_LEN - 1))
        {
            g_lineBuf[g_lineLen++] = (char)byte;
        }
    }
}
