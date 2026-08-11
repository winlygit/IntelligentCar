#include "mode.h"
#include "global.h"
#include "readdata.h"
#include "motor.h"
#include "servo.h"
#include "ik.h"


void mode1_handle(void){
    while(1){
        motorSPEED motorspeed;
        servoANGLE servoangle;
        //读数据
        mode12_data data;
        readdata12(&data, RxData);
        //先判断是否退出
        if(data.IFSTOP == 1) return;

        //主逻辑
        if(data.STATUS == 0){
            motor_stop(&motorspeed);
            servo_stop(&servoangle);
            Motor_Sendcmd(&motorspeed);
            Servo_Sendcmd(&servoangle);
        }else if(data.STATUS == 1){
            motor_ik(&motorspeed, &data.speedData_primary);
            servo_ik(&servoangle, &data.servoData_primary);
            Motor_Sendcmd(&motorspeed);
            Servo_Sendcmd(&servoangle);
        }
        

    }
}

void mode2_handle(void) {
    while(1){
        //读数据
        mode12_data data;
        readdata12(&data, RxData);
        //先判断是否退出
        if(data.IFSTOP == 1) return;

        //主逻辑
        
    }
}

void mode3_handle(void) {
    while(1){
        //读数据
        mode3_data data;
        readdata3(&data, RxData);
        //先判断是否退出
        if(data.IFSTOP == 1) return;

        //主逻辑


    }
}

void mode4_handle(void) {
    while(1){
        //读数据
        mode4_data data;
        readdata4(&data, RxData);
        //先判断是否退出
        if(data.IFSTOP == 1) return;

        //主逻辑


    }
}