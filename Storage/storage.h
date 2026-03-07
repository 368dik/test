#ifndef __STORAGE_H
#define __STORAGE_H

#include "stdint.h"

/* 存储磨损均衡策略 */
typedef enum
{
    STORAGE_WEAR_LEVEL_NONE = 0,
    STORAGE_WEAR_LEVEL_ROUND_ROBIN = 1
} StorageWearLevelMode;

/* 存储记录格式：固定长度，便于Flash顺序写入 */
typedef struct
{
    uint32_t timestampMs;
    uint16_t type;
    uint16_t length;
    uint8_t payload[16];
    uint16_t crc16;
} StorageRecord;

/* 初始化存储模块并恢复已有记录 */
void Storage_Init(StorageWearLevelMode mode);
/* 追加写入一条记录，成功返回1 */
uint8_t Storage_Append(const StorageRecord *record);
/* 获取当前有效记录数 */
uint16_t Storage_GetCount(void);
/* 按索引读取一条记录并校验CRC，成功返回1 */
uint8_t Storage_ReadByIndex(uint16_t index, StorageRecord *outRecord);

#endif
