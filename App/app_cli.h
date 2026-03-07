#ifndef __APP_CLI_H
#define __APP_CLI_H

#include "stdint.h"

void AppCli_Init(void);
void AppCli_OnRxByte(uint8_t byte);
void AppCli_Process(void);

#endif
