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
extern int whether_auto;
int p_mo,i_mo,d_mo;
extern float p,i,d;
extern int connect_flag;
extern char temp_mqtt[50];
extern char to_uno[9];

void read_usart_pc()
{
  unsigned short local=0;
  int pidc=0;
  int pid_s=0; //1-p,2-i,3-d;
  int pp[2];
  int ii[2];
  int dd[2];
  pp[0]=0; pp[1]=0;
  ii[0]=0; ii[1]=0;
  dd[0]=0; dd[1]=0;
  i_pc = Serial.available();  //返回目前串口接收区内的已经接受的数据量
  if(i_pc != 0)
  {
    while(i_pc--)
    {
      temp_pc[local] = Serial.read();   //读取一个数据并且将它从缓存区删除
      local++;
    }

    //目标流速设置
    if(temp_pc[0]=='V' && temp_pc[1]=='E' && temp_pc[2]=='L' && temp_pc[3]=='O')  //"VELO xxx"  data:5 6
    {
      if(!temp_pc[6])
        target_velo=temp_pc[5]-48;
      else if(!temp_pc[7])
        target_velo=(temp_pc[5]-48)*10+temp_pc[6]-48;
      tv = target_velo*1.0;
      pid1.set_target(tv*2);
      to_uno[0]=1+48;
      to_uno[1]=target_velo/10+48;
      to_uno[2]=target_velo%10+48;
      //Serial.println(to_uno);
      SerialUno.print(to_uno);
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
      whether_auto = 0;
      for(pidc=4;pidc<19;pidc++)
      {
        if(temp_pc[pidc]=='P') pid_s=1;
        else if(temp_pc[pidc]=='I') pid_s=2;
        else if(temp_pc[pidc]=='D') pid_s=3;
        
        if(temp_pc[pidc]>='0' && temp_pc[pidc]<='9')
        {
          switch(pid_s)
          {
            case 1:
            {
              pp[0]=pp[1];
              pp[1]=temp_pc[pidc]-48;
              //Serial.print('p');
              break;
            }
            case 2:
            {
              ii[0]=ii[1];
              ii[1]=temp_pc[pidc]-48;
              //Serial.print('i');
              break;
            }
            case 3:
            {
              dd[0]=dd[1];
              dd[1]=temp_pc[pidc]-48;
              //Serial.print('d');
              break;
            }
          }
        }
      }
      to_uno[0]=2+48;
      to_uno[1]=3+48;
      to_uno[2]=4+48;
      to_uno[3]=pp[0]+48; to_uno[4]=pp[1]+48;
      to_uno[5]=ii[0]+48; to_uno[6]=ii[1]+48;
      to_uno[7]=dd[0]+48; to_uno[8]=dd[1]+48;
      SerialUno.print(to_uno);
      p_mo=pp[0]*10+pp[1];
      i_mo=ii[0]*10+ii[1];
      d_mo=dd[0]*10+dd[1];
      p=1.0*p_mo/10; 
      i=1.0*i_mo/10; 
      d=1.0*d_mo/10;
      //Serial.print(" "); Serial.print(pp[0]); Serial.print(" "); Serial.print(pp[1]);
      //Serial.print(" "); Serial.print(ii[0]); Serial.print(" "); Serial.print(ii[1]);
      //Serial.print(" "); Serial.print(dd[0]); Serial.print(" "); Serial.print(dd[1]);
      //Serial.print(" "); Serial.print(p);
      //Serial.print(" "); Serial.print(i);
      //Serial.print(" "); Serial.println(d);
      pid_s=0;
      pid1.set_pid(p,i,d);
    }

    //回环检测
    if(temp_pc[0]=='A' && temp_pc[1]=='N' && temp_pc[2]=='S' && temp_pc[3]=='W')  //"ANSW"
    {
      connect_flag = 1;
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
      velo_i=(temp_uno[0]-48)*1000+(temp_uno[1]-48)*100+(temp_uno[2]-48)*10+temp_uno[3]-48;
      //Serial.println(temp_mqtt);
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
