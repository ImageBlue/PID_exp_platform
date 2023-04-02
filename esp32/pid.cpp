#include "pid.h"
#include<Arduino.h>
using namespace std;
extern int whether_auto;

void pid::set_pid(float p, float i, float d) {
  kp = p;
  ki = i;
  kd = d;
}

//设置target目标(流速)值
void pid::set_target(float velotury) {
  target = velotury;
}

void pid::get_pid(float &p, float &i, float &d){
  p = kp;
  i = ki;
  d = kd;
}

//pid实现,核心算法
float pid::pid_realize(/*float velotury*/) {
  //target = velotury;                                                                //设定目标值
  err = target - actual;                                                        //计算差值
  err_c = err - err_last;
  if (whether_auto)
    fuzzy_pid();
  integral += err;                                                              //累积积分值
  output = kp * err + ki * integral + kd * (err - err_last);                    //通过PID计算输出值
  err_last = err;
  float pwm = 78642 - output*600;                                                                //差值记录
  if(pwm > output_max)
    pwm = output_max;
  if(pwm < output_min)
    pwm = output_min;                                        //算的，感觉行，回头可能会改（看水下实验结果）
  return pwm;                                                                 //返回输出值
}

//获取当前速度值
float pid::get_output() {                                                         //目前代码仅用于作测试
  return output;
}

float pid::set_actual(float act_velo) {
  actual = act_velo;
  return actual;
}

float pid::read_actual(){
  return actual;
}

void pid::get_grad_membership(float qerr, float qerr_c) {
  if (qerr > e_membership_values[0] && qerr < e_membership_values[6])
  {
    for (int i = 0; i < num_area - 2; i++)
    {
      if (qerr >= e_membership_values[i] && qerr <= e_membership_values[i + 1])
      {
        e_gradmembership[0] = -(qerr - e_membership_values[i + 1]) / (e_membership_values[i + 1] - e_membership_values[i]);
        e_gradmembership[1] = 1 + (qerr - e_membership_values[i + 1]) / (e_membership_values[i + 1] - e_membership_values[i]);
        e_grad_index[0] = i;
        e_grad_index[1] = i + 1;
        break;
      }
    }
  }
  else if (qerr <= e_membership_values[0])
  {
    e_gradmembership[0] = 1;
    e_gradmembership[1] = 0;
    e_grad_index[0] = 0;
    e_grad_index[1] = -1;
  }
  else
  {
    e_gradmembership[0] = 1;
    e_gradmembership[1] = 0;
    e_grad_index[0] = 6;
    e_grad_index[1] = -1;
  }

  if (qerr_c > ec_membership_values[0] && qerr_c < ec_membership_values[6])
  {
    for (int i = 0; i < num_area - 2; i++)
    {
      if (qerr_c >= ec_membership_values[i] && qerr_c <= ec_membership_values[i + 1])
      {
        ec_gradmembership[0] = -(qerr_c - ec_membership_values[i + 1]) / (ec_membership_values[i + 1] - ec_membership_values[i]);
        ec_gradmembership[1] = 1 + (qerr_c - ec_membership_values[i + 1]) / (ec_membership_values[i + 1] - ec_membership_values[i]);
        ec_grad_index[0] = i;
        ec_grad_index[1] = i + 1;
        break;
      }
    }
  }
  else if (qerr_c <= ec_membership_values[0])
  {
    ec_gradmembership[0] = 1;
    ec_gradmembership[1] = 0;
    ec_grad_index[0] = 0;
    ec_grad_index[1] = -1;
  }
  else
  {
    ec_gradmembership[0] = 1;
    ec_gradmembership[1] = 0;
    ec_grad_index[0] = 6;
    ec_grad_index[1] = -1;
  }
}

void pid::get_sum_grad()
{
  for (int i = 0; i <= num_area - 1; i++)
  {
    KpgradSums[i] = 0;
    KigradSums[i] = 0;
    KdgradSums[i] = 0;
  }
  for (int i = 0; i < 2; i++)
  {
    if (e_grad_index[i] == -1)
    {
      continue;
    }
    for (int j = 0; j < 2; j++)
    {
      if (ec_grad_index[j] != -1)
      {
        int indexKp = Kp_rule_list[e_grad_index[i]][ec_grad_index[j]] + 3;
        int indexKi = Ki_rule_list[e_grad_index[i]][ec_grad_index[j]] + 3;
        int indexKd = Kd_rule_list[e_grad_index[i]][ec_grad_index[j]] + 3;
        //gradSums[index] = gradSums[index] + (e_gradmembership[i] * ec_gradmembership[j])* Kp_rule_list[e_grad_index[i]][ec_grad_index[j]];
        KpgradSums[indexKp] += (e_gradmembership[i] * ec_gradmembership[j]);
        KigradSums[indexKi] += (e_gradmembership[i] * ec_gradmembership[j]);
        KdgradSums[indexKd] += (e_gradmembership[i] * ec_gradmembership[j]);
      }
      else
      {
        continue;
      }
    }
  }
}

void pid::auto_set_pid()
{
  float qdetail_kp = 0;               //增量kp对应论域中的值
  float qdetail_ki = 0;               //增量ki对应论域中的值
  float qdetail_kd = 0;               //增量kd对应论域中的值
  for (int i = 0; i < num_area - 1; i++)
  {
    qdetail_kp += kp_menbership_values[i] * KpgradSums[i];
    qdetail_ki += ki_menbership_values[i] * KigradSums[i];
    qdetail_kd += kd_menbership_values[i] * KdgradSums[i];
  }
  kp += inverse_quantization(kp_max, kp_min, qdetail_kp);
  ki += inverse_quantization(ki_max, ki_min, qdetail_ki);
  kd += inverse_quantization(kd_max, kd_min, qdetail_kd);
  if (kp < 0)
    kp = 0;
  if (ki < 0)
    ki = 0;
  if (kd < 0)
    kd = 0;
}


void pid::fuzzy_pid() {
  float qerro = quantization(err_max, err_min, err);
  float qerro_c = quantization(errc_max, errc_min, err_c);
  get_grad_membership(qerro, qerro_c);
  get_sum_grad();
  auto_set_pid();
}

float pid::quantization(float maximum, float minimum, float x)
{
  float qvalues = 6.0 * (x - minimum) / (maximum - minimum) - 3;
  return qvalues;
}


float pid::inverse_quantization(float maximum, float minimum, float qvalues)
{
  float x = (maximum - minimum) * (qvalues + 3) / 6 + minimum;
  return x;
}

void pid::set_auto() {
  auto_pid = !auto_pid;
  if (auto_pid)
    Serial.println("Enable fuzzy_pid.");
  else
    Serial.println("Disable fuzzy_pid");
}

bool pid::read_auto(){
  return auto_pid;
}
