/**
 * @file ESP-CAM.ino
 * @author 杨家麒 (2313108048@mail.dlut.edu.cn)
 * @brief 基于开源硬件的水下推进器的PID实验平台 ESP-32部分
 * @version 1.0
 * @date 2023-2-8
 * 
 * @copyright Copyright (c) 2023 杨家麒, 刘崇胜, 唐晨晖
 */

#include "soc/rtc_cntl_reg.h"
#include "soc/soc.h"
#include "SoftwareSerial.h"
#include <HardwareSerial.h>    //导入ESP32串口操作库,使用这个库我们可以把串口映射到其他的引脚上使用
#include "pid.h"

SoftwareSerial SerialUno(22, 23);
pid pid1;                             //PID初始化,设置初始值
command cmd;

int whether_auto = 0;
int target_velo = 0;  //目标流速
float velo = 0.0;
int velo_i;
float p, i, d;
long int pwmval = 0;  //推进器

uint8_t p_pin = 4;
uint8_t led_pinb = 2;
uint8_t led_chb = 2;

void setup() 
{
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
  
  Serial.begin(115200);
  SerialUno.begin(115200);

  ledcSetup(2, 50, 20);
  ledcAttachPin(p_pin, 2);
  ledcWrite(1, 78642);

  ledcSetup(led_chb, 1000, 10); 
  ledcAttachPin(led_pinb, led_chb);
  ledcWrite(led_chb, 1023);
}

void loop() 
{
  read_usart_pc();
  read_usart_uno();
  Serial.print("VELO ");
  Serial.println(velo_i);
  //if(whether_auto) pid1.get_pid(p, i, d);
  //if(whether_auto) ledcWrite(led_chb, 1023);
  //else ledcWrite(led_chb, 0);
  cmd.run_pid(pid1);
  ledcWrite(1, pwmval);
  delay(100);
}
