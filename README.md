# 说明

---

###  最小`MCU`调度系统

- [x] 抢占式
- [x] 队列
- [x] 延时
- [x] 任务返回
- [x] 动态内存
- [x] 定时器
- [x] 信号量
- [x] 互斥量
- [x] 运行统计
- [x] 栈地址监测
- [x] 优先级天花板
- [ ] 参数检查
- [ ] 驱动框架
- [ ] `POSIX`
- [ ] 组件接口
- [ ] 性能、安全、稳定性


### 参考例程编译环境

* `IDE` ` keil-MDK 5.34 `
* 编码 ` UTF-8 without BOM `
* 芯片型号 ` STM32L432KCUx ` `CM4F`
* 编译器版本 ` v6.16 `
* 是否使用微库 ` true `
* 是否使用`FPU` ` true `
* 优化等级 ` any `
* 警告类型  ` AC5-like Warnings `
* 编译标志 ` -xc -std=gnu99 --target=arm-arm-none-eabi -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -c `

### 评估版

* `NUCLEO-L432KC`

`https://www.st.com/content/st_com/en/products/evaluation-tools/product-evaluation-tools/mcu-mpu-eval-tools/stm32-mcu-mpu-eval-tools/stm32-nucleo-boards/nucleo-l432kc.html`




### 参考

* `FreeRTOS`
* `Linux`
