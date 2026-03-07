#ifndef __LOG_H
#define __LOG_H

#include "stdint.h"

void Log_Init(void);
void Log_Write(const char *text);
void Log_WriteLine(const char *text);
void Log_WriteU32(const char *prefix, uint32_t value);

#endif
