本文介绍了如何在芯源的 CW32F03 系列 MCU 中移植 FreeRTOS。

本次使用的芯片是武汉芯源的 CW32F030F8，规格如下：
- 内核：ARM® Cortex®-M0+，最高主频 64MHz
- Flash：64KB 片上存储
- SRAM：8KB

由于只是移植 FreeRTOS，其它的外设就不过多介绍了，有需要了解的可以去看看数据手册。

**⚠️注意：本文旨在用最少的步骤快速进行 FreeRTOS 的移植，不涉及 FreeRTOS 的参数优化**

详细移植过程请阅读：[CW32 移植 FreeRTOS](https://kydins.com/posts/069bc222/)
