#ifndef __SOFT_TIMER_H
#define __SOFT_TIMER_H

#include "stdint.h"

typedef void (*SoftTimerCallback)(void *context);

typedef struct
{
    uint32_t periodMs;
    uint32_t remainMs;
    uint8_t repeat;
    uint8_t active;
    SoftTimerCallback callback;
    void *context;
} SoftTimer;

void SoftTimer_SystemInit(void);
int8_t SoftTimer_Create(uint32_t periodMs, uint8_t repeat, SoftTimerCallback callback, void *context);
void SoftTimer_Start(int8_t timerId);
void SoftTimer_Stop(int8_t timerId);
void SoftTimer_Tick1ms(void);
void SoftTimer_RunPending(void);

#endif
