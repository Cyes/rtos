# 版本说明

---



### 记录

* 有时自己会忘记之前代码是怎么写的，仅做记录

| 版本 | 开始时间     | 记录 | 备注 |
| ---- | ------------ | ---- | ---- |
| 无   | `2021.04.29` |      |      |




### 修改日志

##### 首次实现并跑在`MCU`上



* 2022-05-07 

```
queue 修改： 可以读取写入的字节数
timer 修改： 删除task_list,改为task_desc,因为不需要按照优先级查找，改为另一种阻塞实现方式
demo  修改： 简化例程
mem   添加： os_realloc
task  修改： 加入栈地址检查，加入全动态添加任务，移除不相关元素
sem   修改： put方法可多次执行
其他：添加中断内执行以及判断、添加部分参数检查，中断中最好不要调用引起阻塞的API

```

