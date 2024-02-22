# GR5525-Smart-Watch

[EN](README.md)   |  [中文](README_zh.md)


## 1. 介绍

### 1.1 参考设计

本Smart Watch (智能手表) 参考设计基于Goodix GR5525 BLE SoC进行实现, 设计支持 Lvgl_8.31  GUI框架. 用户的穿戴产品可以通过 GR5525的能力获得更好的帧率和更强的图形能力.


### 1.2 GR5525

GR5525系列是汇顶科技推出的Bluetooth 5.3单模低功耗蓝牙系统级芯片,可配置为广播者、观察者、外围设备或中央设备,并支持上述各种角色的组合应用。GR5525广泛应用于物联网(IoT)、和智能穿戴设备领域。

基于ARM® Cortex®-M4F CPU内核，GR5525系列集成了蓝牙5.3协议栈、2.4 GHz射频收发器、片上可编程Flash存储、RAM及多种外设，提供了多达4路I2C/UART接口，扩展了I/O功能，并增强了QSPI接口，可覆盖更多的IoT、穿戴应用场景。


- 点击 [链接](https://www.goodix.com/zh/product/connectivity/ble/gr5525) 查看 GR5525 介绍

## 2. Smart Watch

### 2.1 工程说明 

参考工程位于 [projects/peripheral/graphics/gr5525_smart_watch](projects/peripheral/graphics/gr5525_smart_watch), 默认由 Keil µVision5 构建. 


### 2.2 构建开发环境

 构建参考工程的开发环境, 需要依赖以下软件: 

- Keil µVision5: 参考工程默认的编译构建IDE
- Device Family Pack (DFP) : 支持 ARM Cortex-M4 FPU 的 DFP包
- J-Link: 程序下载调试工具
- 串口助手: 用于程序的串口日志打印
- GR5525 keil 下载算法文件:  .flm 算法文件默认放置在目录 [build/keil](build/keil)

### 2.3 硬件相关

- 本Smart Watch参考设计基于 GR5525 SK开发板进行,  用户可以从这里找到关于开发板的介绍  [GR5525_SK](https://www.goodix.com/zh/kit/gr5525_starter_kit)
- 点击 [GR5525_Diagram](https://www.goodix.com/zh/docview/GR5525-SK-BASIC-RevB_V1.0?objectId=351&objectType=document&version=541) 查看 GR5525 SK 板的电路图, 用户可以参考SK电路图, 进行自己的二次开发修改. 进行产品开发或 DIY 

### 2.4 软件相关

- 当前参考工程引用的开发包基于 GR5525 SDK v1.0.0
- Lvgl 版本基于 Lvgl 8.31 进行优化

### 2.5 参考设计的演示效果 

- 可以通过下面方式观看参考设计的演示效果(更多界面) :

  - 点击跳转到B站观看 : [Smart_Watch_Video](https://www.bilibili.com/video/BV1Re411X7P6/?share_source=copy_web&vd_source=253f7e2d634ff4f728c7e7bfa218f990)
  -  使用手机扫描演示视频的二维码观看. 

