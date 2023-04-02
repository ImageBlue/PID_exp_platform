#include<WiFi.h>
#include<PubSubClient.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

//const char* ssid ="_OurEDA_OurFi";               //ESP32连接的WiFi账号
//const char* password =  "OurEDA2021";        //WiFi密码
//const char* mqttServer = "172.6.2.71";  //要连接到的服务器IP
const char* ssid ="emmmmmm";               //ESP32连接的WiFi账号
const char* password =  "317348asdfgh";        //WiFi密码
const char* mqttServer = "192.168.43.74";  //要连接到的服务器IP
const int mqttPort = 1883;                 //要连接到的服务器端口号
const char* mqttUser = "TEST";            //MQTT服务器账号
const char* mqttPassword = "test";       //MQTT服务器密码
char temp[50];
char rec[50];
double num;
int wifi_flag = 0;
extern int connect_flag;
extern int led_flag;
extern char to_uno[9];
extern float velo;
extern int target_velo;
float mtv = 0.0;
extern int whether_auto;
int mp_o,mi_o,md_o;
extern float p,i,d;
int p_o,i_o,d_o;
int mqtt_flag=0;  //mqtt是否初始化

void callback(char*topic, byte* payload, unsigned int length) 
{
  char mess[50];
  int pidc=0;
  int pid_s=0; //1-p,2-i,3-d;
  int pp[2];
  int ii[2];
  int dd[2];
  pp[0]=0; pp[1]=0;
  ii[0]=0; ii[1]=0;
  dd[0]=0; dd[1]=0;
  for(int i = 0; i< length; i++)        //使用循环打印接收到的信息
  {
    mess[i]=(char)payload[i];
    Serial.print(mess[i]);
  }
  Serial.println(' ');

  //目标流速设置
  if(mess[0]=='V' && mess[1]=='E' && mess[2]=='L' && mess[3]=='O')  //"VELO xxx"  data:5 6
  {
    target_velo=(mess[5]-'0')*10+mess[6]-'0';
    mtv = target_velo*1.0;
    Serial.println(target_velo);
    Serial.println(mtv);
    pid1.set_target(mtv*2);
    to_uno[0]=1+48;
    to_uno[1]=target_velo/10+48;
    to_uno[2]=target_velo%10+48;
    SerialUno.print(to_uno);
  }

  //自动功能识别
  if(mess[0]=='A' && mess[1]=='U' && mess[2]=='T' && mess[3]=='O')  //"AUTO"
  {
    whether_auto = 1;
  }

  //手动功能识别
  if(mess[0]=='M' && mess[1]=='A' && mess[2]=='N' && mess[3]=='U')  //"MANU"
  {
    whether_auto = 0;
  }

  //PID参数设置（手动）
  if(mess[0]=='P' && mess[1]=='I' && mess[2]=='D')  //“PID P:xx I:xx D:xx"  data: 4+2/3 9+ 14+
  {
      whether_auto = 0;
      for(pidc=4;pidc<19;pidc++)
      {
        if(mess[pidc]=='P') pid_s=1;
        else if(mess[pidc]=='I') pid_s=2;
        else if(mess[pidc]=='D') pid_s=3;
        
        if(mess[pidc]>='0' && mess[pidc]<='9')
        {
          switch(pid_s)
          {
            case 1:
            {
              pp[0]=pp[1];
              pp[1]=mess[pidc]-48;
              //Serial.print('p');
              break;
            }
            case 2:
            {
              ii[0]=ii[1];
              ii[1]=mess[pidc]-48;
              //Serial.print('i');
              break;
            }
            case 3:
            {
              dd[0]=dd[1];
              dd[1]=mess[pidc]-48;
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
      p_o=pp[0]*10+pp[1];
      i_o=ii[0]*10+ii[1];
      d_o=dd[0]*10+dd[1];
      p=1.0*p_o/10; 
      i=1.0*i_o/10; 
      d=1.0*d_o/10;
      //Serial.print(" "); Serial.print(pp[0]); Serial.print(" "); Serial.print(pp[1]);
      //Serial.print(" "); Serial.print(ii[0]); Serial.print(" "); Serial.print(ii[1]);
      //Serial.print(" "); Serial.print(dd[0]); Serial.print(" "); Serial.print(dd[1]);
      //Serial.print(" "); Serial.print(p);
      //Serial.print(" "); Serial.print(i);
      //Serial.print(" "); Serial.println(d);
      pid_s=0;
      pid1.set_pid(p,i,d);
  }
}

void wifi_connect()   //wifi连接
{
  int i = 0;
  if(!wifi_flag)
  {
    WiFi.begin(ssid,password);          //接入WiFi函数（WiFi名称，密码）重新连接wifi
    for(i=0; i<50; i++)
    {
      if(WiFi.status() == WL_CONNECTED)
      {
        wifi_flag = 1;
        //Serial.print(wifi_flag);
        //Serial.println("waiting for wifi");
        break;
      }
      delay(200);
      led_flag = (led_flag==0) ? 1023 : 0;
      ledcWrite(led_chb, led_flag);
    }
  }
  else if(wifi_flag && !mqtt_flag)
  {
    client.setServer(mqttServer,mqttPort);  //MQTT服务器连接函数（服务器IP，端口号）
    client.setCallback(callback);           //设定回调方式，当ESP32收到订阅消息时会调用此方法
    mqtt_flag=1;
  }
}

void mqtt_connect()
{
  int j;
  if(wifi_flag)
  {
    for(j=0;j<3;j++)
    {
      if(!client.connected())
      {
        if(client.connect("ESP32Client",mqttUser, mqttPassword ))  //如果服务器连接成功
        {
          connect_flag = 2;
          client.subscribe("Command");                 //连接MQTT服务器后订阅主题
          mqtt_flag=0;
          break;
        }
        else 
        {
          //delay(200);  
          //Serial.println("waiting for mqtt"); 
          led_flag = (led_flag==0) ? 1023 : 0;   
          ledcWrite(led_chb, led_flag);                  
        }
      }
    }
  }
}
