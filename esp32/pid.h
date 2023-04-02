#include<Arduino.h>
#ifndef pid_h
#define pid_h

#define NB -3
#define NM -2
#define NS -1
#define ZO 0
#define SP 1
#define PM 2
#define PB 3


//pid类
class pid {
  private:
    float kp, ki, kd;   //pid参数

    float actual;       //当前值(实际值)
    float target;       //目标值

    float err;          //偏差
    float err_last;     //上一个偏差值
    float err_c;        //偏差差值
    float err_max;      //偏差最大值
    float err_min;      //偏差最小值
    float errc_max;     //偏差差值最大值
    float errc_min;     //偏差差值最小值

    //pid参数范围:防止过大/过小
    float kp_max;       
    float kp_min;
    float ki_max;
    float ki_min;
    float kd_max;
    float kd_min;

    bool auto_pid;    //是否开启模糊pid

    float integral;     //积分
    float output;      //pwm输出值

    //输出的pwm的最大值和最小值    
    float output_max;
    float output_min;

    int num_area = 8;

    float e_gradmembership[2];      //输入e的隶属度
    float ec_gradmembership[2];     //输入de/dt的隶属度
    int e_grad_index[2];            //输入e隶属度在规则表的索引
    int ec_grad_index[2];           //输入de/dt隶属度在规则表的索引
    float gradSums[7] = {0, 0, 0, 0, 0, 0, 0};
    float KpgradSums[7] = { 0, 0, 0, 0, 0, 0, 0 }; //输出增量kp总的隶属度
    float KigradSums[7] = { 0, 0, 0, 0, 0, 0, 0 }; //输出增量ki总的隶属度
    float KdgradSums[7] = { 0, 0, 0, 0, 0, 0, 0 }; //输出增量kd总的隶属度


    float e_membership_values[7] = { -3, -2, -1, 0, 1, 2, 3}; //输入e的隶属值
    float ec_membership_values[7] = { -3, -2, -1, 0, 1, 2, 3 }; //输入de/dt的隶属值
    float kp_menbership_values[7] = { -3, -2, -1, 0, 1, 2, 3 }; //输出增量kp的隶属值
    float ki_menbership_values[7] = { -3, -2, -1, 0, 1, 2, 3 }; //输出增量ki的隶属值
    float kd_menbership_values[7] = { -3, -2, -1, 0, 1, 2, 3 }; //输出增量kd的隶属值
    float fuzzyoutput_menbership_values[7] = { -3, -2, -1, 0, 1, 2, 3 };


    int  Kp_rule_list[7][7] = {       //kp规则表
      {PB, PB, PM, PM, SP, ZO, ZO},
      {PB, PB, PM, SP, SP, ZO, NS},
      {PM, PM, PM, SP, ZO, NS, NS},
      {PM, PM, SP, ZO, NS, NM, NM},
      {SP, SP, ZO, NS, NS, NM, NM},
      {SP, ZO, NS, NM, NM, NM, NB},
      {ZO, ZO, NM, NM, NM, NB, NB}
    };

    int  Ki_rule_list[7][7] = {       //ki规则表
      {NB, NB, NM, NM, NS, ZO, ZO},
      {NB, NB, NM, NS, NS, ZO, ZO},
      {NB, NM, NS, NS, ZO, SP, SP},
      {NM, NM, NS, ZO, SP, PM, PM},
      {NM, NS, ZO, SP, SP, PM, PB},
      {ZO, ZO, SP, SP, PM, PB, PB},
      {ZO, ZO, SP, PM, PM, PB, PB}
    };

    int  Kd_rule_list[7][7] = {       //kd规则表
      {SP, NS, NB, NB, NB, NM, SP},
      {SP, NS, NB, NM, NM, NS, ZO},
      {ZO, NS, NM, NM, NS, NS, ZO},
      {ZO, NS, NS, NS, NS, NS, ZO},
      {ZO, ZO, ZO, ZO, ZO, ZO, ZO},
      {PB, NS, SP, SP, SP, SP, PB},
      {PB, PM, PM, PM, SP, SP, PB}
    };

    int  Fuzzy_rule_list[7][7] = {
      {PB, PB, PB, PB, PM, ZO, ZO},
      {PB, PB, PB, PM, PM, ZO, ZO},
      {PB, PM, PM, SP, ZO, NS, NM},
      {PM, PM, SP, ZO, NS, NM, NM},
      {SP, SP, ZO, NM, NM, NM, NB},
      {ZO, ZO, ZO, NM, NB, NB, NB},
      {ZO, NS, NB, NB, NB, NB, NB}
    };

  public:
  
    
    pid(float p = 0.2, float i = 0.1, float d = 0.08) {
      kp = p;
      ki = i;
      kd = d;
      target = 0;

      auto_pid = 0;

      actual = 0.0;
      err = 0;
      err_last = 0;
      err_c = 0;
      integral = 0.0;

      output = 0;

      err_max = 1000;
      err_min = -1000;
      errc_max = 800;
      errc_min = -800;


      kp_max = 0.1;
      kp_min = -0.1;
      ki_max = 0.1;
      ki_min = -0.1;
      kd_max = 0.01;
      kd_min = -0.01;

      output_max = 78642;
      output_min = 52429;      
    }

    //设置pid参数
    void set_pid(float p, float i, float d);

    //设置target目标(流速)值
    void set_target(float velotury);

    //pid实现,核心算法，
    float pid_realize(/*float velotury*/);

    //获取当前速度值
    float get_output();

    //读取当前pid
    void get_pid(float &p, float &i, float &d);

    //输入实际流速值
    float set_actual(float act_velo);

    //返回当前实际流速值(没什么用)
    float read_actual();

    //模糊pid的实现
    void fuzzy_pid();

    //开启/关闭模糊pid;
    void set_auto();

    //读取auto_pid状态
    bool read_auto();

    //以下模糊pid相关函数
    void get_grad_membership(float qerr, float qerr_c);

    void get_sum_grad();

    void auto_set_pid();

    float quantization(float maximum, float minimum, float x);

    float inverse_quantization(float maximum, float minimum, float qvalues);

};


class command{
  private:
    String input;
    String instruction;
  
  public:
    
    //检查是否有从上位机接收到字符形式指令，并按照指令对pid类进行操作后，然后运行pid
    void receive(pid);

    //调用流速函数获取当前流速
    float get_actual();

    //运行pid
    void run_pid(pid);

    void output_pwm(float);

    //清除输入输出字符串
    void str_clean() {
      input = "";
      instruction = "";
    }

};

#endif
