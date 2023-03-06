#include"pid.h"

extern float velo;
extern long int pwmval;

/*入口入口入口入口*/
float command::get_actual(){
    //调用从流速计读取当前流速的函数并且返回  
    return velo;
}

/*出口出口出口出口*/
void command::output_pwm(float pwm){
    //向电机发送pwm
    pwmval=pwm;
}

void command::receive(pid pid1){
    while (Serial.available() > 0) {
      char rcv = Serial.read();                   //读取串口数据到字符串input中
      input += rcv;
      delay(100);
      //Serial.println(inData);
      if (rcv == '\n')                            //读取完成时
      {
        //Serial.println("get");
        instruction = input.substring(0, 4);
        Serial.println(instruction);
        if (instruction == "MANU") {
          float p, i, d;
          int pos1 = input.indexOf('P');
          int pos2 = input.indexOf('I');
          p = input.substring(pos1 + 1, pos2 - 1).toFloat();
          pos1 = input.indexOf('D');
          i = input.substring(pos2 + 1, pos1 - 1).toFloat();
          //pos2 = input.indexOf('E');
          d = input.substring(pos1 + 1).toFloat();
          //Serial.println(input.substring(pos1 + 1));
          Serial.printf("P:%.3f\tI:%.3f\tD:%.3f\r\n", p, i, d);
          pid1.set_pid(p, i, d);
          Serial.printf("Set PID succeed.\r\n");
          str_clean();
        }
        else if (instruction == "AUTO") {
          pid1.set_auto();
          //Serial.printf("前面的区域,以后再来探索吧!\r\n");
          //auto_set();                             //通过模糊pid设置参数
          str_clean();
        }
        else if (instruction == "VELO") {
          String tmp = input.substring(5);
          pid1.set_target(tmp.toFloat());
          Serial.printf("Set velotury succeed.\r\n");
          str_clean();
        }
        else {
          Serial.printf("Error input...");
          str_clean();
        }
      }
    }
    if (pid1.read_auto()) {
      float p, i, d;
      pid1.get_pid(p, i, d);
      //Serial.printf("Fuzzy pid control is working...\r\nCurrent P : %f\tI : %f\tD : %f\r\n", p, i, d);
    }
    run_pid(pid1);

}

    //运行pid
void command::run_pid(pid pid1){
  float actual = get_actual();
  pid1.set_actual(actual);
  float output = pid1.pid_realize();
  //int act = int(actual);
  //Serial.printf("VELO \r\n%d",  act);                   //.\t\tOutput is %f.\r\n     , output
  //Serial.printf
  output_pwm(output);
}
