#include "mode.h"
#include "global.h"
#include "readdata.h"
#include "motor.h"
#include "servo.h"
#include "ik.h"
#include "actiongroup.h"
#include "line.h"
#include "uart.h"
#include <string.h>
#include <stdio.h>


void mode1_handle(void){
    motorSPEED motorspeed;
    servoANGLE servoangle;
    
    mode1_data data;
    data.IFSTOP = 0;
    data.STATUS = 0;

    U3_printf("@ACKM1#");

    while(1){
        if(rxcplt_flag == 1){
            //读数据
            readdata1(&data, RxData);
            rxcplt_flag = 0;
            ifrxstart = 0;
        }
        //先判断是否退出
        if(data.IFSTOP == 1) {
            U3_printf("@ACKST#");
            motor_stop(&motorspeed);
            servo_stop(&servoangle);
            Motor_Sendcmd(&motorspeed);
            Servo_Sendcmd(&servoangle);
            return;}

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

    U3_printf("@ACKM2#");

    mode2_data data;
    data.IFSTOP = 0;
    data.STATUS = 0;

    motorSPEED motorspeed;
    servoANGLE servoangle;

    uint32_t last_record_tick = 0;
    const uint32_t record_each_ms = 50; /* 每50ms记录一步 (换算最大持续录制时间约24s，可考虑调整) */


    while(1){
        if(rxcplt_flag == 1){
            //读数据
            readdata2(&data, RxData);
            rxcplt_flag = 0;
            ifrxstart = 0;
        }

        //先判断是否退出
        if(data.IFSTOP == 1){   
            U3_printf("@ACKST#");        
            motor_stop(&motorspeed);
            servo_stop(&servoangle);
            Motor_Sendcmd(&motorspeed);
            Servo_Sendcmd(&servoangle);
            if(ActionGroup_IsRecording() == 1) ActionGroup_StopRecord();
            return;
        }

        //主逻辑
        if(data.STATUS == 0){ 
            if(ActionGroup_IsRecording() == 1){ //若正在录制，则停止录制并保存
                motor_stop(&motorspeed);
                servo_stop(&servoangle);
                Motor_Sendcmd(&motorspeed);
                Servo_Sendcmd(&servoangle);
                ActionGroup_StopRecord();
            }
    }
        else if(data.STATUS == 1 && ActionGroup_IsRecording() == 0){   // 启动录制
            char name[16] = {0};
            strncpy(name, (char*)data.NAME, 16);
            name[15] = '\0'; // 保证结尾
            if(ActionGroup_StartRecord(name, strlen(name)) != 0) {
                char msg[64];
                sprintf(msg, "Failed to start recording for group: %s\n", name);
                U3_printf((uint8_t*)msg); // 发送错误信息
                } else {
                    last_record_tick = HAL_GetTick();
                }
        }

        if(data.STATUS == 1 && ActionGroup_IsRecording() == 1){
            motor_ik(&motorspeed, &data.speedData_primary);
            servo_ik(&servoangle, &data.servoData_primary);
            Motor_Sendcmd(&motorspeed);
            Servo_Sendcmd(&servoangle);

            uint32_t now = HAL_GetTick();
            if(now - last_record_tick >= record_each_ms){
                ActionGroup_RecordStep(&motorspeed, &servoangle, data.servoData_primary.D6);
                last_record_tick = now;
            }
        }
    }
}

void mode3_handle(void) {

    U3_printf("@ACKM3#");

    mode3_data data;
    data.STATUS = 0;
    data.IFSTOP = 0;

    uint32_t addr;

    char msg[64];

    while(1){
        __WFI();
        if(rxcplt_flag == 1){
            memset(&data, 0, sizeof(data));
            //读数据
            readdata3(&data, RxData);
            rxcplt_flag = 0;
            ifrxstart = 0;
        }
        //先判断是否退出
        if(data.IFSTOP == 1) {
            U3_printf("@ACKST#");
            motor_stop(&motorspeed);
            servo_stop(&servoangle);
            Motor_Sendcmd(&motorspeed);
            Servo_Sendcmd(&servoangle);
            return;
        }

        //主逻辑
        if(data.IFREFRESH == 1 && data.STATUS == 0){
            data.STATUS = 1;
            // 刷新发送动作组目录
            ActionGroup_List();
            data.IFREFRESH = 0; // ？？？
            data.STATUS = 0;
        }
        else if(data.IFEXECUTE == 1 && data.STATUS == 0){
            data.STATUS = 1;
            // 执行动作组
            if(ActionGroup_Find(data.ACTIONID, &addr)){
                motorSPEED motorspeed;
                servoANGLE servoangle;
                ActionGroup_Play(addr, &motorspeed, &servoangle);
                sprintf(msg, "Successfully executing Group %d\n", data.ACTIONID);
                U3_printf((uint8_t*)msg);
            }
            else{
                sprintf(msg, "Group %d not found\n", data.ACTIONID);
                U3_printf((uint8_t*)msg);
            }
            data.IFEXECUTE = 0;
            data.STATUS = 0;
        }
        else if(data.IFDELETE == 1 && data.STATUS == 0){
            // 删除动作组
            if(ActionGroup_Find(data.DELETEID, &addr)){
                ActionGroup_Delete(data.DELETEID);
                sprintf(msg, "Successfully deleted Group %d\n", data.DELETEID);
                U3_printf((uint8_t*)msg);
            }
            else{
                sprintf(msg, "Group %d not found\n", data.DELETEID);
                U3_printf((uint8_t*)msg);
                data.STATUS = 0;
            }
            data.IFDELETE = 0;
            data.STATUS = 0;
        }


    }
}


void mode4_handle(void)
{

    U3_printf("@ACKM4#");

    mode4_data data;
    data.IFSTOP = 0;
    data.IFREFRESH = 0;
    data.STATUS = 0;
    data.IFREACH = 0;

    int16_t speed = 300;

    char msg[64];

    uint32_t addr;

    motorSPEED motorspeed;
    servoANGLE servoangle;

    while(1)
    {
        if(rxcplt_flag == 1)
        {
            readdata4(&data, RxData);
            rxcplt_flag = 0;
            ifrxstart = 0;
        }

        if(data.IFSTOP == 1) {
            U3_printf("@ACKST#");
            motor_stop(&motorspeed);
            servo_stop(&servoangle);
            Motor_Sendcmd(&motorspeed);
            Servo_Sendcmd(&servoangle);
            return;
        }

        if(data.IFREFRESH == 1 && data.STATUS == 0){
            data.STATUS = 1;
            // 刷新动作组目录
            ActionGroup_List();
            data.IFREFRESH = 0;
            data.STATUS = 0;
        }
        if(data.STATUS == 1)
        {
            //获取实际距离
            int16_t actual_distance;
            // *********actual_distance = GetDistance();    ***传感器
            //检查是否达到阈值
            data.IFREACH = IFREACH_check(actual_distance, data.DISTANCE);

            if(data.IFREACH == 0)  //未达到，继续巡线
            {
                LINE_Track(&motorspeed, speed);
                Motor_Sendcmd(&motorspeed);
            }
            else  //达到阈值，执行动作组
            {
                motor_stop(&motorspeed);
                Motor_Sendcmd(&motorspeed);
                Servo_Sendcmd(&servoangle);

                if(ActionGroup_Find(data.ACTIONID, &addr))
                {
                    ActionGroup_Play(addr, &motorspeed, &servoangle);
                    sprintf(msg, "Successfully executing Group %d\n", data.ACTIONID);
                    U3_printf((uint8_t*)msg);
                }
                else
                {
                    sprintf(msg, "Group %d not found\n", data.ACTIONID);
                    U3_printf((uint8_t*)msg);
                }
                data.STATUS = 0;
                data.IFREACH = 0;
            }
        }

        }
    }
