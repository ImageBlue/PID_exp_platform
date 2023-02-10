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

const int input = 8;
int X;
int Y;
float TIME = 0;
float FREQUENCY = 0.0;
float WATER = 0;
float TOTAL = 0;
float LS = 0;
long int f = 0;
float f_lcd = 0.0;

const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;  // 定义引脚
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);   // 根据引脚，创建一个 lcd 实例

SoftwareSerial SerialESP(A4, A5);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(input, INPUT);
  // 初始化串口
  SerialESP.begin(115200);
  Serial.begin(115200);
  lcd.begin(16, 2); //   初始化lcd ，并定义LCD1602的规格
}

void loop() {
  digitalWrite(LED_BUILTIN,HIGH);

  f=re_velo()*100;
  if(f<10){
    SerialESP.print(0); SerialESP.print(0); SerialESP.print(0); SerialESP.print(0);
    //Serial.print(0); Serial.print(0); Serial.print(0); Serial.print(0);
  }else if(f<100) {
    SerialESP.print(0); SerialESP.print(0); SerialESP.print(0);
    //Serial.print(0); Serial.print(0); Serial.print(0); Serial.print(0);
  }else if(f<1000){
    SerialESP.print(0); Serial.print(0);
    //Serial.print(0); Serial.print(0); 
  }else if(f<10000){
    SerialESP.print(0);
    //Serial.print(0);
  }
  SerialESP.print(f);
  //Serial.println(f);

  f_lcd = 1.0*f/100;
  lcd_pro(TOTAL,f_lcd);
  
  delay(100);
}

float re_velo()
{
  X = pulseIn(input, HIGH);
  Y = pulseIn(input, LOW);
  TIME = X + Y + 1;
  FREQUENCY = 1000000 / TIME;
  if(FREQUENCY != 1000000)
  {
    WATER = FREQUENCY / 7.5;
    LS = WATER / 60;
    TOTAL = TOTAL + LS;
  }
  if(FREQUENCY!=1000000 && FREQUENCY>0) 
    return(FREQUENCY);
  else
    return 0;
}

void lcd_pro(float tol,float num){
  lcd.clear();
  lcd.setCursor(0,0); //   设置指针 在x=0，y=1的位置
  lcd.print("tol");  //   lcd 显示字符串
  lcd.setCursor(4,0); //   设置指针 在x=5，y=1的位置
  lcd.print(tol); //   lcd 显示字符串
  lcd.setCursor(15,0); //   设置指针 在x=12，y=1的位置
  lcd.print("L"); //   lcd 显示字符串
  lcd.setCursor(0,1); //   设置指针 在x=0，y=1的位置
  lcd.print("velo");  //   lcd 显示字符串
  lcd.setCursor(5,1); //   设置指针 在x=5，y=1的位置
  lcd.print(num); //   lcd 显示字符串
  lcd.setCursor(12,1); //   设置指针 在x=12，y=1的位置
  lcd.print("L/h"); //   lcd 显示字符串
}
