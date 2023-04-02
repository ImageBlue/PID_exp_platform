/**
 * @file ESP-CAM.ino
 * @author 杨家麒 (2313108048@mail.dlut.edu.cn)
 * @brief 基于开源硬件的水下推进器的PID实验平台 ESP-32部分
 * @version 1.0
 * @date 2023-2-8
 * 
 * @copyright Copyright (c) 2023 杨家麒, 刘崇胜, 唐晨晖
 */
#include<WiFi.h>
#include<PubSubClient.h>
#include "soc/rtc_cntl_reg.h"
#include "soc/soc.h"
#include "SoftwareSerial.h"
#include <HardwareSerial.h>    //导入ESP32串口操作库,使用这个库我们可以把串口映射到其他的引脚上使用
#include "pid.h"

SoftwareSerial SerialUno(22, 23);  //rx tx
pid pid1;                             //PID初始化,设置初始值
command cmd;
WiFiClient espClient;                     // 定义wifiClient实例
PubSubClient client(espClient);          // 定义PubSubClient的实例

int connect_flag = 0; //0-no 1-uart 2-mqtt
int led_flag = 0; //0 or 1023
char temp_ret[10]; //mqtt
char to_uno[9];

int whether_auto = 1;
int target_velo = 0;  //目标流速
float velo = 0.0;
int velo_i = 0;
float p, i, d;
long int pwmval = 0;  //推进器

uint8_t p_pin = 4;
uint8_t p_ch=1; 
uint8_t led_pinb = 2;
uint8_t led_chb = 2;
uint8_t mode_pin = 15;
uint8_t mode_ch = 3;
const int buttonPin = 5;     // the number of the pushbutton pin
int buttonState = 0;         // variable for reading the pushbutton status

void check_connect()  //连接检测
{
  int c=0;
  wifi_connect();
  mqtt_connect();
  for(c=0;c<10;c++)
  {
    Serial.println("CALL");
    delay(200);
    led_flag = (led_flag==0) ? 1023 : 0;
    ledcWrite(led_chb, led_flag); 
    read_usart_pc();
    if(connect_flag==1) break;
  }

  if(connect_flag == 1)
    ledcWrite(led_chb, 0);  //串口
  else if(connect_flag == 2)
    ledcWrite(led_chb, 1023);  //mqtt
}

void write_temp()
{
  temp_ret[0]='V';
  temp_ret[1]='E';
  temp_ret[2]='L';
  temp_ret[3]='O';
  temp_ret[4]=' ';
  temp_ret[5]=velo_i/1000+48;
  temp_ret[6]=(velo_i/100)%10+48;
  temp_ret[7]=(velo_i/10)%10+48;
  temp_ret[8]=velo_i%10+48;
}

void setup() 
{
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
  
  Serial.begin(115200);
  SerialUno.begin(115200);

  //pinMode(buttonPin, INPUT);

  ledcSetup(p_ch, 50, 20);
  ledcAttachPin(p_pin, p_ch);
  ledcWrite(p_ch, 78642);

  ledcSetup(mode_ch, 1000, 10); 
  ledcAttachPin(mode_pin, mode_ch);
  ledcWrite(mode_ch, 1023);

  ledcSetup(led_chb, 1000, 10); 
  ledcAttachPin(led_pinb, led_chb);
  ledcWrite(led_chb, 1023);
}

void loop() 
{
  if(!connect_flag)
    check_connect();
  else
  {
    if(connect_flag == 1)
      read_usart_pc();
    else if(connect_flag == 2)
      client.loop();
      
    read_usart_uno();
    write_temp();
    
    if(connect_flag == 1)
      Serial.println(temp_ret);
    else if(connect_flag == 2)
      client.publish("Callback",temp_ret);
    memset(temp_ret,'\0',sizeof(temp_ret));
    
    if(whether_auto) 
      pid1.get_pid(p, i, d);
    if(whether_auto) 
      ledcWrite(mode_ch, 1023);
    else 
      ledcWrite(mode_ch, 0);

    //buttonState = digitalRead(buttonPin);
    //if(!buttonState)
      //connect_flag = 0;
    //else
      //connect_flag = 1;
      
    cmd.run_pid(pid1);
    //Serial.println(pwmval);
    ledcWrite(1, pwmval);   //推进器
    delay(200);
  }
}
