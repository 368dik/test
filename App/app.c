#include "app.h"

#include "OLED.h"
#include "Timer.h"
#include "soft_timer.h"
#include "log.h"
#include "bsp_uart.h"
#include "storage.h"
#include "app_cli.h"
#include "drv_dht11.h"

/* 应用主状态机 */
typedef enum
{
    APP_STATE_BOOT = 0,
    APP_STATE_IDLE,
    APP_STATE_RUN
} AppState;

#define APP_SAMPLE_PERIOD_MS 2000

static AppState g_appState;
static uint32_t g_heartbeat;
static int8_t g_heartbeatTimerId;

/* 2秒周期任务：采集DHT11、刷新OLED并写入Flash */
static void App_OnHeartbeat(void *context)
{
    DHT11_Data dhtData;
    StorageRecord record;

    (void)context;
    g_heartbeat++;

    if (DHT11_Read(&dhtData))
    {
        OLED_ShowString(1, 1, "T:");
        OLED_ShowNum(1, 3, dhtData.temperature, 2);
        OLED_ShowString(1, 5, "C H:");
        OLED_ShowNum(1, 9, dhtData.humidity, 2);
        OLED_ShowString(1, 11, "%");

        record.timestampMs = g_heartbeat * APP_SAMPLE_PERIOD_MS;
        record.type = 2;
        record.length = 2;
        record.payload[0] = dhtData.temperature;
        record.payload[1] = dhtData.humidity;
        {
            uint8_t index;
            for (index = 2; index < sizeof(record.payload); index++)
            {
                record.payload[index] = 0;
            }
        }
        Storage_Append(&record);
    }
    else
    {
        OLED_ShowString(1, 1, "DHT11 ERROR    ");
        OLED_ShowString(3, 1, "ERR:");
        OLED_ShowNum(3, 5, DHT11_GetLastError(), 2);
        Log_WriteU32("dht11_err=", DHT11_GetLastError());
    }

    OLED_ShowString(2, 1, "REC:");
    OLED_ShowNum(2, 5, Storage_GetCount(), 5);
    Log_WriteU32("heartbeat=", g_heartbeat);
}

/* 应用初始化：外设、存储、CLI和定时任务 */
void App_Init(void)
{
    StorageRecord lastRecord;

    g_appState = APP_STATE_BOOT;
    g_heartbeat = 0;

    Timer_Init();
    SoftTimer_SystemInit();

    Log_Init();
    OLED_Init();
    OLED_Clear();
    DHT11_Init();

    Storage_Init(STORAGE_WEAR_LEVEL_ROUND_ROBIN);

    if (Storage_GetCount() > 0)
    {
        if (Storage_ReadByIndex((uint16_t)(Storage_GetCount() - 1), &lastRecord) && lastRecord.type == 2 && lastRecord.length >= 2)
        {
            OLED_ShowString(1, 1, "T:");
            OLED_ShowNum(1, 3, lastRecord.payload[0], 2);
            OLED_ShowString(1, 5, "C H:");
            OLED_ShowNum(1, 9, lastRecord.payload[1], 2);
            OLED_ShowString(1, 11, "%");
        }
    }

    OLED_ShowString(2, 1, "REC:");
    OLED_ShowNum(2, 5, Storage_GetCount(), 5);

    AppCli_Init();

    g_heartbeatTimerId = SoftTimer_Create(APP_SAMPLE_PERIOD_MS, 1, App_OnHeartbeat, 0);
    SoftTimer_Start(g_heartbeatTimerId);

    g_appState = APP_STATE_IDLE;
    Log_WriteLine("app ready");
}

/* 应用主循环：处理串口输入与软定时任务 */
void App_Loop(void)
{
    uint8_t rxByte;

    if (g_appState == APP_STATE_IDLE)
    {
        g_appState = APP_STATE_RUN;
    }

    while (BSP_UART1_TryReadByte(&rxByte))
    {
        AppCli_OnRxByte(rxByte);
    }

    AppCli_Process();
    SoftTimer_RunPending();
}
