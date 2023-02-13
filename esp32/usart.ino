#include "SoftwareSerial.h"
#include <HardwareSerial.h>    //导入ESP32串口操作库,使用这个库我们可以把串口映射到其他的引脚上使用
#include "pid.h"

unsigned short i_pc;   //usart pc
unsigned short i_uno;  //usart uno
char temp_pc [50];
char temp_uno[50];
extern float velo;
extern int target_velo;
float tv = 0.0;
extern int velo_i;
extern int whether_auto;
int p_o,i_o,d_o;
extern float p,i,d;

void read_usart_pc()
{
  unsigned short local=0;
  i_pc = Serial.available();  //返回目前串口接收区内的已经接受的数据量
  if(i_pc != 0)
  {
    while(i_pc--)
    {
      temp_pc[local] = Serial.read();   //读取一个数据并且将它从缓存区删除
      local++;
    }

    //目标流速设置
    if(temp_pc[0]=='V' && temp_pc[1]=='E' && temp_pc[2]=='L' && temp_pc[3]=='O')  //"VELO xxx"  data:5 6 7
    {
      if(!temp_pc[6])
        target_velo=temp_pc[5]-48;
      else if(!temp_pc[7])
        target_velo=(temp_pc[5]-48)*10+temp_pc[6]-48;
      else if(!temp_pc[8])
        target_velo=(temp_pc[5]-48)*100+(temp_pc[6]-48)*10+temp_pc[7]-48;
      tv = target_velo*1.0;
      pid1.set_target(tv);
    }

    //自动功能识别
    if(temp_pc[0]=='A' && temp_pc[1]=='U' && temp_pc[2]=='T' && temp_pc[3]=='O')  //"AUTO"
    {
      whether_auto = 1;
    }

    //手动功能识别
    if(temp_pc[0]=='M' && temp_pc[1]=='A' && temp_pc[2]=='N' && temp_pc[3]=='U')  //"MANU"
    {
      whether_auto = 0;
    }

    //PID参数设置（手动）
    if(temp_pc[0]=='P' && temp_pc[1]=='I' && temp_pc[2]=='D')  //“PID P:xx I:xx D:xx"  data: 4+2/3 9+ 14+
    {
      if(!temp_pc[4]=='P')
        p_o=(temp_pc[6]-48)*10+temp_pc[7]-48;
      if(!temp_pc[9]=='I')
        i_o=(temp_pc[11]-48)*10+temp_pc[12]-48;
      if(!temp_pc[14]=='D')
        d_o=(temp_pc[16]-48)*10+temp_pc[17]-48;
      p=1.0*p_o/10; i_o=1.0*i_o/10; d_o=1.0*d_o/10;
      pid1.set_pid(p,i,d);
    }
    
    //Serial.println(target_velo); 
    memset(temp_pc,'\0',sizeof(temp_pc));
  }
}

void read_usart_uno()
{
  unsigned short int local_uno;
  i_uno = SerialUno.available();  //返回目前串口接收区内的已经接受的数据量
  if(i_uno!=0)
  {
    while(i_uno--)
    {
      temp_uno[local_uno] = SerialUno.read();   //读取一个数据并且将它从缓存区删除
      if(temp_uno[local_uno]>57 || temp_uno[local_uno]<48)
        temp_uno[local_uno] = '0';
      //Serial.print(temp_uno[local_uno]);
      local_uno++;
      //Serial.print(0);
    }
    
    //if(temp_uno[local_uno]='\n'){
      //Serial.print(" ");
      //Serial.print(" ");
      //Serial.print(temp_uno);
      //Serial.print("  ");
      velo_i=(temp_uno[0]-48)*10000+(temp_uno[1]-48)*1000+(temp_uno[2]-48)*100+(temp_uno[3]-48)*10+temp_uno[4]-48;
      velo=1.0*velo_i/100;
      //velo_i=velo_i%100;
      //Serial.print("VELO ");
      //Serial.println(velo_i);
      //Serial.print(velo_i);
      //Serial.print(" ");
      //Serial.println(velo);
      memset(temp_uno,'\0',sizeof(temp_uno));  
      //local_uno = 0;
    //}
  }
}
