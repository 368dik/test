#ifndef __RING_BUFFER_H
#define __RING_BUFFER_H

#include "stdint.h"

typedef struct
{
    uint8_t *buffer;          /* 由调用者提供的外部存储区 */
    uint16_t size;            /* buffer[] 总槽位数 */
    volatile uint16_t head;   /* 下一次写入位置 */
    volatile uint16_t tail;   /* 下一次读取位置 */
} RingBuffer;

/* 使用调用者提供的内存初始化环形缓冲区。 */
void RingBuffer_Init(RingBuffer *rb, uint8_t *mem, uint16_t size);
/* 写入 1 字节，成功返回 1，缓冲区满返回 0。 */
uint8_t RingBuffer_Push(RingBuffer *rb, uint8_t data);
/* 读取 1 字节，成功返回 1，缓冲区空返回 0。 */
uint8_t RingBuffer_Pop(RingBuffer *rb, uint8_t *outData);
/* 当前环形缓冲区中的字节数。 */
uint16_t RingBuffer_Count(const RingBuffer *rb);
/* 快速判空辅助函数。 */
uint8_t RingBuffer_IsEmpty(const RingBuffer *rb);

#endif
