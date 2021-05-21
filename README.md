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
- [ ]  动态内存管理
- [ ]  信号量和互斥量
- [ ]  优先级反转解决
- [ ]  性能、安全、稳定性
- [ ]  驱动框架、参数检查和栈地址监测

### 参考

* FreeRTOS
* RT-Thread
* LiteOS
* Linux



### 例程描述

* 共五个任务

| 任务名称  | 优先级 | 运行间隔-毫秒 | 功能                                                | 任务ID |
| --------- | ------ | ------------- | --------------------------------------------------- | ------ |
| task1     | 1      | 300           | LED 闪烁                                            | 0      |
| task2     | 2      | 1000          | 向队列写入字符串 `task2`， 20秒后return（删除自己） | 1      |
| task3     | 2      | 100           | 读队列并打印                                        | 2      |
| task4     | 3      | 2000          | 向队列写入字符串 `task4` 和 `浮点数据`              | 3      |
| task_idle | 31     | 0             | 死循环                                              | 4      |



### 打印信息

* serial:115200-N-8-1

```
run 
task3[ task2 ]
task3[ task4 = 0.020243 ]
task3[ task2 ]
task3[ task2 ]
task3[ task4 = 0.040486 ]
task3[ task2 ]
task3[ task2 ]
task3[ task4 = 0.060728 ]
.
.
.
task3[ task2 ]
task3[ task2 ]
task3[ task4 = 0.222671 ]
task3[ ** task2  bye ** ]
task3[ task4 = 0.242914 ]
task3[ task4 = 0.263157 ]
task3[ task4 = 0.283399 ]
task3[ task4 = 0.303642 ]
task3[ task4 = 0.323885 ]
```

### 修改日志
* 修改寻找最高优先级算法，使用`CLZ`，优先级支持`0 ~ 31`
* 加入抢占式调度
* 未测试优先级执行顺序
* 未测试时间片轮转