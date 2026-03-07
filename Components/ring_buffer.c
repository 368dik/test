#include "ring_buffer.h"

void RingBuffer_Init(RingBuffer *rb, uint8_t *mem, uint16_t size)
{
    rb->buffer = mem;
    rb->size = size;
    rb->head = 0;
    rb->tail = 0;
}

uint8_t RingBuffer_Push(RingBuffer *rb, uint8_t data)
{
    uint16_t nextHead = rb->head + 1;
    if (nextHead >= rb->size)
    {
        nextHead = 0;
    }

    if (nextHead == rb->tail)
    {
        return 0;
    }

    rb->buffer[rb->head] = data;
    rb->head = nextHead;
    return 1;
}

uint8_t RingBuffer_Pop(RingBuffer *rb, uint8_t *outData)
{
    if (rb->head == rb->tail)
    {
        return 0;
    }

    *outData = rb->buffer[rb->tail];
    rb->tail++;
    if (rb->tail >= rb->size)
    {
        rb->tail = 0;
    }
    return 1;
}

uint16_t RingBuffer_Count(const RingBuffer *rb)
{
    if (rb->head >= rb->tail)
    {
        return rb->head - rb->tail;
    }

    return (rb->size - rb->tail) + rb->head;
}

uint8_t RingBuffer_IsEmpty(const RingBuffer *rb)
{
    return rb->head == rb->tail;
}
