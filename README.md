# PID_exp_platform
某小组的大创项目 A PID experiment platform based on open-sourced hardware.

项目名称：基于开源硬件的水下推进器PID实验平台

项目的代码主要分为两部分
OController：上位机
  主要功能：显示实时流速、与下位机有线或无线连接、对下位机PID参数和目标流速的设置

下位机：
  主要功能：控制推进器、响应上位机发来的请求  
  
下位机主要由外壳、shield、Arduino UNO、ESP32、各模块组成  
shield连接各个模块，外壳则进行保护包装
