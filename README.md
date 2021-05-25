# 说明

---

###  最小MCU调度系统

- [x]  基于优先级的抢占式任务调度
- [x]  队列和阻塞
- [x]  延时和阻塞
- [x]  任务创建和删除

### 编译环境

* IDE ` keil-MDK 5.34 `
* 编码 ` UTF-8 without BOM `
* 硬件平台 ` STM32L432KCUx `
* 编译器版本 ` v6.16 `
* 是否使用微库 ` true `
* 是否使用FPU ` true `
* 优化等级 ` any `
* 警告类型  ` AC5-like Warnings `
* 编译标志 ` -xc -std=gnu99 --target=arm-arm-none-eabi -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -c `

### 硬件平台

* NUCLEO-L432KC

`https://www.st.com/content/st_com/en/products/evaluation-tools/product-evaluation-tools/mcu-mpu-eval-tools/stm32-mcu-mpu-eval-tools/stm32-nucleo-boards/nucleo-l432kc.html`




### 未实现的功能

- [ ]  定时器
- [x]  动态内存管理
- [ ]  信号量和互斥量
- [ ]  优先级反转解决
- [ ]  性能、安全、稳定性
- [ ]  驱动框架、参数检查和栈地址监测


### 参考

* FreeRTOS
* RT-Thread
* LiteOS
* Linux


### 修改日志
* 修改寻找最高优先级算法，使用`CLZ`，优先级支持`0 ~ 31`
* 加入抢占式调度
* 修改队列稳定性
* 引入bug（SYSTICK优先级只能为0）
* 加入任务统计功能
* 修改时钟和临界段相关