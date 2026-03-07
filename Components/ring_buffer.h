#ifndef __RING_BUFFER_H
#define __RING_BUFFER_H

#include "stdint.h"

typedef struct
{
    uint8_t *buffer;
    uint16_t size;
    volatile uint16_t head;
    volatile uint16_t tail;
} RingBuffer;

void RingBuffer_Init(RingBuffer *rb, uint8_t *mem, uint16_t size);
uint8_t RingBuffer_Push(RingBuffer *rb, uint8_t data);
uint8_t RingBuffer_Pop(RingBuffer *rb, uint8_t *outData);
uint16_t RingBuffer_Count(const RingBuffer *rb);
uint8_t RingBuffer_IsEmpty(const RingBuffer *rb);

#endif
