#include "ring_buffer.h"

/* head == tail 表示空；为区分满/空，始终预留一个空槽位。 */
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
        /* 写指针到达末尾后回卷到 0。 */
        nextHead = 0;
    }

    /* 满条件：写指针前进一步就会追上读指针。 */
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
    /* 空条件：head 与 tail 重合，没有可读数据。 */
    if (rb->head == rb->tail)
    {
        return 0;
    }

    *outData = rb->buffer[rb->tail];
    rb->tail++;
    if (rb->tail >= rb->size)
    {
        /* 读指针到达末尾后回卷到 0。 */
        rb->tail = 0;
    }
    return 1;
}

uint16_t RingBuffer_Count(const RingBuffer *rb)
{
    /* 未回卷：head 在 tail 右侧。 */
    if (rb->head >= rb->tail)
    {
        return rb->head - rb->tail;
    }

    /* 已回卷：数量 = [tail..end) + [0..head)。 */
    return (rb->size - rb->tail) + rb->head;
}

uint8_t RingBuffer_IsEmpty(const RingBuffer *rb)
{
    return rb->head == rb->tail;
}
