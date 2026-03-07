#include "soft_timer.h"

#define SOFT_TIMER_MAX_COUNT 8

static SoftTimer g_softTimers[SOFT_TIMER_MAX_COUNT];
static volatile uint8_t g_softTimerPending[SOFT_TIMER_MAX_COUNT];

void SoftTimer_SystemInit(void)
{
    uint8_t index;
    for (index = 0; index < SOFT_TIMER_MAX_COUNT; index++)
    {
        g_softTimers[index].periodMs = 0;
        g_softTimers[index].remainMs = 0;
        g_softTimers[index].repeat = 0;
        g_softTimers[index].active = 0;
        g_softTimers[index].callback = 0;
        g_softTimers[index].context = 0;
        g_softTimerPending[index] = 0;
    }
}

int8_t SoftTimer_Create(uint32_t periodMs, uint8_t repeat, SoftTimerCallback callback, void *context)
{
    uint8_t index;

    for (index = 0; index < SOFT_TIMER_MAX_COUNT; index++)
    {
        if (g_softTimers[index].callback == 0)
        {
            g_softTimers[index].periodMs = periodMs;
            g_softTimers[index].remainMs = periodMs;
            g_softTimers[index].repeat = repeat;
            g_softTimers[index].active = 0;
            g_softTimers[index].callback = callback;
            g_softTimers[index].context = context;
            g_softTimerPending[index] = 0;
            return (int8_t)index;
        }
    }

    return -1;
}

void SoftTimer_Start(int8_t timerId)
{
    if (timerId < 0 || timerId >= SOFT_TIMER_MAX_COUNT)
    {
        return;
    }

    if (g_softTimers[timerId].callback == 0)
    {
        return;
    }

    g_softTimers[timerId].remainMs = g_softTimers[timerId].periodMs;
    g_softTimers[timerId].active = 1;
}

void SoftTimer_Stop(int8_t timerId)
{
    if (timerId < 0 || timerId >= SOFT_TIMER_MAX_COUNT)
    {
        return;
    }

    g_softTimers[timerId].active = 0;
}

void SoftTimer_Tick1ms(void)
{
    uint8_t index;

    for (index = 0; index < SOFT_TIMER_MAX_COUNT; index++)
    {
        if (g_softTimers[index].active == 0 || g_softTimers[index].callback == 0)
        {
            continue;
        }

        if (g_softTimers[index].remainMs > 0)
        {
            g_softTimers[index].remainMs--;
        }

        if (g_softTimers[index].remainMs == 0)
        {
            g_softTimerPending[index] = 1;

            if (g_softTimers[index].repeat)
            {
                g_softTimers[index].remainMs = g_softTimers[index].periodMs;
            }
            else
            {
                g_softTimers[index].active = 0;
            }
        }
    }
}

void SoftTimer_RunPending(void)
{
    uint8_t index;

    for (index = 0; index < SOFT_TIMER_MAX_COUNT; index++)
    {
        if (g_softTimerPending[index])
        {
            g_softTimerPending[index] = 0;
            g_softTimers[index].callback(g_softTimers[index].context);
        }
    }
}
