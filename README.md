# 说明

---

###  最小MCU调度系统

- [x] 基于优先级的抢占式任务调度
- [x] 队列
- [x] 延时
- [x] 任务返回
- [x] 动态内存
- [ ] 定时器
- [ ] 信号量
- [ ] 互斥量
- [x] 运行统计
- [ ] 参数检查
- [x] 栈地址监测
- [ ] 优先级反转
- [ ] 驱动框架
- [ ] POSIX
- [ ] 组件接口
- [ ] 性能、安全、稳定性


### 编译环境

* IDE ` keil-MDK 5.34 `
* 编码 ` UTF-8 without BOM `
* 芯片型号 ` STM32L432KCUx ` `CM4F`
* 编译器版本 ` v6.16 `
* 是否使用微库 ` true `
* 是否使用FPU ` true `
* 优化等级 ` any `
* 警告类型  ` AC5-like Warnings `
* 编译标志 ` -xc -std=gnu99 --target=arm-arm-none-eabi -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -c `

### 评估版

* NUCLEO-L432KC

`https://www.st.com/content/st_com/en/products/evaluation-tools/product-evaluation-tools/mcu-mpu-eval-tools/stm32-mcu-mpu-eval-tools/stm32-nucleo-boards/nucleo-l432kc.html`




### 参考

* `FreeRTOS`
* `Linux`


### 修改日志
* 修改寻找最高优先级算法，使用`CLZ`，优先级支持`0 ~ 31`
* 加入抢占式调度
* 修改队列稳定性
* 重置`PendSV`和`SysTick`优先级
* 加入任务统计功能
* 修改时钟和临界段相关
* 基于数组的内存块，实现内存合并
* 拆分任务移动的函数