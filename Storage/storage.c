#include "storage.h"
#include "drv_flash.h"

/* 仅使用Flash起始4KB作为记录区 */
#define STORAGE_FLASH_BASE_ADDR 0x000000
#define STORAGE_FLASH_SECTOR_SIZE 4096
#define STORAGE_SLOT_SIZE ((uint32_t)sizeof(StorageRecord))
#define STORAGE_MAX_RECORDS ((uint16_t)(STORAGE_FLASH_SECTOR_SIZE / STORAGE_SLOT_SIZE))

/* 运行时状态 */
static uint16_t g_recordCount;
static uint16_t g_nextWriteIndex;
static StorageWearLevelMode g_wearLevelMode;

/* CRC16(Modbus)校验 */
static uint16_t Storage_CalcCrc16(const uint8_t *data, uint16_t length)
{
    uint16_t crc = 0xFFFF;
    uint16_t i;
    uint8_t bit;

    for (i = 0; i < length; i++)
    {
        crc ^= data[i];
        for (bit = 0; bit < 8; bit++)
        {
            if (crc & 0x0001)
            {
                crc = (crc >> 1) ^ 0xA001;
            }
            else
            {
                crc >>= 1;
            }
        }
    }

    return crc;
}

/* 将记录索引映射为Flash绝对地址 */
static uint32_t Storage_IndexToAddr(uint16_t index)
{
    return STORAGE_FLASH_BASE_ADDR + ((uint32_t)index * STORAGE_SLOT_SIZE);
}

/* 判断该记录槽是否为擦除态(全0xFF) */
static uint8_t Storage_IsErasedRecord(const StorageRecord *record)
{
    const uint8_t *data = (const uint8_t *)record;
    uint16_t index;

    for (index = 0; index < sizeof(StorageRecord); index++)
    {
        if (data[index] != 0xFF)
        {
            return 0;
        }
    }

    return 1;
}

/* 校验记录CRC有效性 */
static uint8_t Storage_IsRecordValid(const StorageRecord *record)
{
    uint16_t crc = Storage_CalcCrc16((const uint8_t *)record, (uint16_t)(sizeof(StorageRecord) - sizeof(record->crc16)));
    return (uint8_t)(crc == record->crc16);
}

/* 初始化并扫描现有记录，恢复记录数 */
void Storage_Init(StorageWearLevelMode mode)
{
    StorageRecord temp;
    uint16_t index;

    g_wearLevelMode = mode;
    Flash_Init();

    g_recordCount = 0;
    g_nextWriteIndex = 0;

    for (index = 0; index < STORAGE_MAX_RECORDS; index++)
    {
        Flash_Read(Storage_IndexToAddr(index), (uint8_t *)&temp, sizeof(StorageRecord));

        if (Storage_IsErasedRecord(&temp))
        {
            break;
        }

        if (!Storage_IsRecordValid(&temp))
        {
            break;
        }

        g_recordCount++;
        g_nextWriteIndex = g_recordCount;
    }

    if (g_nextWriteIndex >= STORAGE_MAX_RECORDS)
    {
        g_nextWriteIndex = STORAGE_MAX_RECORDS;
    }
}

/* 追加一条记录，空间满时可按策略覆盖 */
uint8_t Storage_Append(const StorageRecord *record)
{
    StorageRecord temp;

    temp = *record;
    temp.crc16 = Storage_CalcCrc16((const uint8_t *)&temp, (uint16_t)(sizeof(StorageRecord) - sizeof(temp.crc16)));

    if (g_recordCount < STORAGE_MAX_RECORDS)
    {
        Flash_Write(Storage_IndexToAddr(g_recordCount), (const uint8_t *)&temp, sizeof(StorageRecord));
        g_recordCount++;
        g_nextWriteIndex = g_recordCount;
        return 1;
    }

    if (g_wearLevelMode == STORAGE_WEAR_LEVEL_ROUND_ROBIN)
    {
        Flash_EraseSector4K(STORAGE_FLASH_BASE_ADDR);
        Flash_Write(Storage_IndexToAddr(0), (const uint8_t *)&temp, sizeof(StorageRecord));
        g_recordCount = 1;
        g_nextWriteIndex = 1;
        return 1;
    }

    return 0;
}

/* 返回当前有效记录条数 */
uint16_t Storage_GetCount(void)
{
    return g_recordCount;
}

/* 按索引读取记录并执行CRC校验 */
uint8_t Storage_ReadByIndex(uint16_t index, StorageRecord *outRecord)
{
    if (index >= g_recordCount)
    {
        return 0;
    }

    Flash_Read(Storage_IndexToAddr(index), (uint8_t *)outRecord, sizeof(StorageRecord));

    return Storage_IsRecordValid(outRecord);
}
