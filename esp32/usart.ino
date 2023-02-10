#include "SoftwareSerial.h"
#include <HardwareSerial.h>    //导入ESP32串口操作库,使用这个库我们可以把串口映射到其他的引脚上使用

unsigned short i_pc;   //usart pc
unsigned short i_uno;  //usart uno
char temp_pc [50];
char temp_uno[50];
extern float velo;
extern int velo_i;
int local_uno = 0;

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
    if(temp_pc[0]=='V' && temp_pc[1]=='E' && temp_pc[2]=='L' && temp_pc[3]=='O')  //"VELO xxx"  data:5 6 7
    {
      if(!temp_pc[6])
        target_velo=temp_pc[5]-48;
      else if(!temp_pc[7])
        target_velo=(temp_pc[5]-48)*10+temp_pc[6]-48;
      else if(!temp_pc[8])
        target_velo=(temp_pc[5]-48)*100+(temp_pc[6]-48)*10+temp_pc[7]-48;
    }
    
    //Serial.println(target_velo); 
    memset(temp_pc,'\0',sizeof(temp_pc));
  }
}

void read_usart_uno()
{
  i_uno = SerialUno.available();  //返回目前串口接收区内的已经接受的数据量
  if(i_uno!=0)
  {
    while(i_uno--)
    {
      temp_uno[local_uno] = SerialUno.read();   //读取一个数据并且将它从缓存区删除
      if(temp_uno[local_uno]>57 || temp_uno[local_uno]<48)
        temp_uno[local_uno] = '0';
      local_uno++;
    }
    
    if(local_uno == 5){
      //Serial.print(temp_uno);
      //Serial.print("  ");
      velo_i=(temp_uno[0]-48)*10000+(temp_uno[1]-48)*1000+(temp_uno[2]-48)*100+(temp_uno[3]-48)*10+temp_uno[4]-48;
      velo=1.0*velo_i/100;
      //Serial.print("VELO ");
      //Serial.println(velo_i);
      memset(temp_uno,'\0',sizeof(temp_uno));  
      local_uno = 0;
    }
  }
}
