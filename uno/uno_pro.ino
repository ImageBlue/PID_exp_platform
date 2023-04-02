/**
 * @file ESP-CAM.ino
 * @author 杨家麒 (2313108048@mail.dlut.edu.cn)
 * @brief 基于开源硬件的水下推进器的PID实验平台 UNO部分
 * @version 1.0
 * @date 2023-2-8
 * 
 * @copyright Copyright (c) 2023 杨家麒, 刘崇胜, 唐晨晖
 */
 
#include <SoftwareSerial.h>
#include <LiquidCrystal.h> // 加载LCD1602的库
#include "MPU9250.h"

const int input = 8;

float roll,pitch,yaw;
int incline;
int tar = 0;
char send_num[7];
float p,i,d;

const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;  // 定义引脚

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);   // 根据引脚，创建一个 lcd 实例
SoftwareSerial SerialESP(A2, A3);
MPU9250 mpu;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(input, INPUT);
  SerialESP.begin(115200);
  //-Serial.begin(115200);
  Wire.begin();
  delay(2000);
  mpu.setup();
  lcd.begin(16, 2); //   初始化lcd ，并定义LCD1602的规格
}         

void loop() {
  digitalWrite(LED_BUILTIN,HIGH);

  mpu.update();
  //mpu.print();
  roll=mpu.getRoll();
  //pitch=mpu.getPitch();
  //yaw=mpu.getYaw();
  incline=9000+100*1.0*roll;
  //Serial.print(incline);
  //Serial.print("  ");

  num_deal(incline);
  //Serial.println(send_num);
  SerialESP.print(send_num);
  lcd_pro(tar,1.0*incline/100,p,i,d);
  read_usart_esp();
  
  delay(200);
}

void read_usart_esp()
{
  unsigned short int local_uno;
  unsigned short int i_uno;
  char temp_uno[9];
  i_uno = SerialESP.available();  //返回目前串口接收区内的已经接受的数据量
  if(i_uno!=0)
  {
    while(i_uno--)
    {
      temp_uno[local_uno] = SerialESP.read();   //读取一个数据并且将它从缓存区删除
      //Serial.print(temp_uno[local_uno]);
      local_uno++;
      //Serial.print(0);
    }
    //Serial.println(temp_uno);
    if(temp_uno[0]-64-48==1)
    {
      tar=(temp_uno[1]-64-48)*10+temp_uno[2]-48-64;
      //Serial.println(tar);
    }
    else if(temp_uno[0]-64-48==2)
    {
      p=temp_uno[3]-64-48+1.0*(temp_uno[4]-48-64)/10;
      i=temp_uno[5]-64-48+1.0*(temp_uno[6]-48-64)/10;
      d=temp_uno[7]-64-48+1.0*(temp_uno[8]-48-64)/10;
      /*
      Serial.print(temp_uno[3]-64-48);
      Serial.print(" ");
      Serial.print(temp_uno[4]-48-64);
      Serial.print(" ");
      Serial.print(temp_uno[5]-64-48);
      Serial.print(" ");
      Serial.print(temp_uno[6]-48-64);
      Serial.print(" ");
      Serial.print(temp_uno[7]-64-48);
      Serial.print(" ");
      Serial.print(temp_uno[8]-48-64);
      Serial.print(" ");
      Serial.print(p);
      Serial.print(" ");
      Serial.print(i);
      Serial.print(" ");
      Serial.println(d);
      */
    }
    //Serial.print(temp_uno);
    //Serial.print("  ");
    memset(temp_uno,'\0',sizeof(temp_uno));  
  }
}

void num_deal(int f){
  if(f<0) 
  {
    send_num[0]=0+48;
    send_num[1]=0+48;
    send_num[2]=0+48;
    send_num[3]=0+48;
  }
  else
  {
    send_num[0]=f/1000+48; 
    send_num[1]=(f%1000)/100+48; 
    send_num[2]=(f%100)/10+48; 
    send_num[3]=f%10+48; 
  }
}

void lcd_pro(int tar, float roll, float p, float i, float d)
{
  lcd.clear();
  lcd.setCursor(0,0); //   设置指针 在x=0，y=1的位置
  lcd.print("r:");  //   lcd 显示字符串
  lcd.setCursor(2,0); //   设置指针 在x=0，y=1的位置
  lcd.print(roll);  //   lcd 显示字符串
  lcd.setCursor(9,0); //   设置指针 在x=0，y=1的位置
  lcd.print("tar:");  //   lcd 显示字符串
  lcd.setCursor(13,0); //   设置指针 在x=0，y=1的位置
  lcd.print(tar);  //   lcd 显示字符串
  lcd.setCursor(0,1); //   设置指针 在x=0，y=1的位置
  lcd.print(":");  //   lcd 显示字符串
  lcd.setCursor(2,1); //   设置指针 在x=0，y=1的位置
  lcd.print(p);  //   lcd 显示字符串
  lcd.setCursor(7,1); //   设置指针 在x=0，y=1的位置
  lcd.print(i);  //   lcd 显示字符串
  lcd.setCursor(12,1); //   设置指针 在x=0，y=1的位置
  lcd.print(i);  //   lcd 显示字符串
}
