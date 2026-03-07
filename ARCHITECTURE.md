# STM32 项目分层说明（优化版）

## 1. BSP（板级支持包）

职责：这块板怎么连（GPIO / 时钟 / 中断 / DMA / 底层收发）

- `BSP/bsp_i2c.c` `BSP/bsp_i2c.h`
  - 提供软件 I2C1（PB6/PB7）底层时序
  - 负责 GPIO 与时钟初始化
- `BSP/bsp_uart.c` `BSP/bsp_uart.h`
  - 提供 UART1 底层初始化与字节收发
  - 提供 RX 中断接收入口
  - 提供 UART1 TX DMA 发送入口

约束：BSP 只做硬件寄存器相关，不写业务逻辑。

## 2. DriverExt（外设驱动扩展）

职责：某个器件怎么用（寄存器表 / 命令集）

- `DriverExt/OLED.c` `DriverExt/OLED.h`
  - 只保留 OLED 指令/显示协议
  - I2C 底层已下沉到 BSP，通过 `bsp_i2c.h` 调用

约束：DriverExt 不直接操作板级 GPIO 引脚。

## 3. Storage（产品能力层）

职责：记录格式、写入策略、磨损均衡（可选）

- `Storage/storage.c` `Storage/storage.h`
  - `StorageRecord`：统一记录结构（时间戳、类型、长度、负载、CRC）
  - `Storage_Append`：追加写入
  - `StorageWearLevelMode`：
    - `STORAGE_WEAR_LEVEL_NONE`
    - `STORAGE_WEAR_LEVEL_ROUND_ROBIN`

当前为 RAM 模拟后端，后续可替换成 Flash/EEPROM，不影响上层接口。

## 4. Components（通用组件）

职责：日志、Ring Buffer、软定时器

- `Components/log.c` `Components/log.h`
  - 文本日志输出与数值日志输出
- `Components/ring_buffer.c` `Components/ring_buffer.h`
  - 通用字节环形缓冲区
- `Components/soft_timer.c` `Components/soft_timer.h`
  - 1ms Tick 驱动的软定时任务调度

## 5. App（业务流程层）

职责：状态机、任务、CLI

- `App/app.c` `App/app.h`
  - 业务状态机（BOOT/IDLE/RUN）
  - 周期任务（心跳显示与日志）
- `App/app_cli.c` `App/app_cli.h`
  - UART CLI 命令处理：`help` `ping` `stat` `clear`
- `App/main.c`
  - 主入口仅保留 `App_Init()` + `App_Loop()`

## 6. 中断与调度

- `App/stm32f10x_it.c`
  - `TIM2_IRQHandler()` 中调用 `SoftTimer_Tick1ms()`
- `Components/Timer.c`
  - TIM2 已配置为 1ms 更新中断

## 7. 你需要在 Keil 工程里加入的新文件

如果 `*.uvprojx` 还没自动包含新增文件，请手动加入：

- `App/app.c`
- `App/app_cli.c`
- `Components/log.c`
- `Components/ring_buffer.c`
- `Components/soft_timer.c`
- `Storage/storage.c`

并确认对应头文件路径已在 Include Path 中。

## 8. 后续建议

- 将 `Storage` 后端从 RAM 替换为 Flash 页面管理
- 在 `BSP_UART1_EnableRxIRQ` 中接入 DMA+IDLE，提升 CLI 吞吐
- 为 `App CLI` 加上参数解析和命令注册表
