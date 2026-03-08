#ifndef __SOFT_TIMER_H
#define __SOFT_TIMER_H

#include "stdint.h"

typedef void (*SoftTimerCallback)(void *context);

typedef struct
{
    uint32_t periodMs;            /* 定时周期（毫秒） */
    uint32_t remainMs;            /* 距离到期的剩余时间 */
    uint8_t repeat;               /* 1：周期定时，0：单次定时 */
    uint8_t active;               /* 1：运行中，0：已停止 */
    SoftTimerCallback callback;   /* 到期回调函数 */
    void *context;                /* 回调时传入的用户上下文 */
} SoftTimer;

/* 重置全部软件定时器槽位与 pending 标志。 */
void SoftTimer_SystemInit(void);
/* 创建定时器，成功返回 timerId(>=0)，失败返回 -1。 */
int8_t SoftTimer_Create(uint32_t periodMs, uint8_t repeat, SoftTimerCallback callback, void *context);
/* 按 id 启动或重启定时器。 */
void SoftTimer_Start(int8_t timerId);
/* 按 id 停止定时器。 */
void SoftTimer_Stop(int8_t timerId);
/* 每 1ms 调用一次（通常在中断中）：更新倒计时并置位 pending。 */
void SoftTimer_Tick1ms(void);
/* 在主循环调用：执行 Tick1ms 置位的待处理回调。 */
void SoftTimer_RunPending(void);

#endif
