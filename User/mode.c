#include "mode.h"
#include "global.h"
#include "readdata.h"
#include "motor.h"
#include "servo.h"
#include "ik.h"
#include "actiongroup.h"
#include "uart.h"
#include <string.h>
#include <stdio.h>
#include "ultrasonic.h"


void mode1_handle(void){
    motorSPEED motorspeed;
    servoANGLE servoangle;
    
    mode1_data data;
    data.IFSTOP = 0;
    data.STATUS = 0;

    U3_printf((uint8_t*)"@ACKM1#");
    data.speedData_primary.Vx = 0;
    data.speedData_primary.Vy = 0;
    data.speedData_primary.Wz = 0;
    data.servoData_primary.D1 = 135;
    data.servoData_primary.D2 = 90;
    data.servoData_primary.D3 = 90;
    data.servoData_primary.D4 = 90;
    data.servoData_primary.D5 = 135;
    data.servoData_primary.D6 = 0;

    while(1){
        if(rxcplt_flag == 1){
            //读数据
            readdata1(&data, RxData);
            rxcplt_flag = 0;
            ifrxstart = 0;
        }
        //先判断是否退出
        if(data.IFSTOP == 1) {
            U3_printf((uint8_t*)"@ACKST#");
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

    

    mode2_data data;
    data.IFSTOP = 0;
    data.STATUS = 0;

    motorSPEED motorspeed;
    servoANGLE servoangle;
    
    U3_printf((uint8_t*)"@ACKM2#");

    uint32_t last_record_tick = 0;
    const uint32_t record_each_ms = 50; /* 每50ms记录一步 (换算最大持续录制时间约35s，可考虑调整) */
    data.speedData_primary.Vx = 0;
    data.speedData_primary.Vy = 0;
    data.speedData_primary.Wz = 0;
    data.servoData_primary.D1 = 135;
    data.servoData_primary.D2 = 90;
    data.servoData_primary.D3 = 90;
    data.servoData_primary.D4 = 90;
    data.servoData_primary.D5 = 135;
    data.servoData_primary.D6 = 0;


    while(1){
        if(rxcplt_flag == 1){
            //读数据
            readdata2(&data, RxData);
            rxcplt_flag = 0;
            ifrxstart = 0;
        }

        //先判断是否退出
        if(data.IFSTOP == 1){
            U3_printf((uint8_t*)"@ACKST#");        
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
                U3_printf((uint8_t*)"@ACKM2ST#"); // 应答结束录制
            }
    }
        else if(data.STATUS == 1 && ActionGroup_IsRecording() == 0){   // 启动录制
            char name[16] = {0};
            strncpy(name, (char*)data.NAME, 16);
            name[15] = '\0'; // 保证结尾
            if(ActionGroup_StartRecord(name, strlen(name)) != 0) {
                char msg[64];
                sprintf(msg, "Fail %s", name); // *********只能抓包，上位机无法收到
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
                ActionGroup_RecordStep(&servoangle);
                last_record_tick = now;
            }
        }
    }
}

void mode3_handle(void) {

    U3_printf((uint8_t*)"@ACKM3#");

    mode3_data data;
    data.STATUS = 0;
    data.IFSTOP = 0;

    motorSPEED motorspeed;
    servoANGLE servoangle;
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
            U3_printf((uint8_t*)"@ACKST#");
            motor_stop(&motorspeed);
            servo_stop(&servoangle);
            Motor_Sendcmd(&motorspeed);
            Servo_Sendcmd(&servoangle);
            return;
        }

        //主逻辑如下

        //请求刷新动作组
        if(data.IFREFRESH == 1 && data.STATUS == 0){
            data.STATUS = 1;
            // 刷新发送动作组目录
            ActionGroup_List_3();
            data.IFREFRESH = 0; // ？？？
            data.STATUS = 0;
        }
        else if(data.IFEXECUTE == 1 && data.STATUS == 0){
            data.STATUS = 1;
            // 执行动作组
            if(ActionGroup_Find(data.ACTIONID, &addr)){
                ActionGroup_Play(addr, &servoangle);
                sprintf(msg, "@ACKM3B%03d#", data.ACTIONID); // 执行成功应答
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
                sprintf(msg, "@ACKM3D%03d#", data.DELETEID);
                U3_printf((uint8_t*)msg);
            }
            else{
                sprintf(msg, "Group %d not found\n", data.DELETEID);// *********只能抓包，上位机无法收到
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
    //进入模式4应答：@M4# 由手机APP经蓝牙(UART2)触发主分发进入本函数；
    //这句 @ACKM4# 经 UART3 发给 OpenMV，作为 OpenMV“开始工作”的启动信号。
    U3_printf((uint8_t*)"@ACKM4#");
    mode4_data data;
    memset(&data, 0, sizeof(data));
    data.speedData_primary.Vx = 0;
    data.speedData_primary.Vy = 0;
    data.speedData_primary.Wz = 0;
    data.servoData_primary.D1 = 45;                 //与 OpenMV 中位 NEUTRAL 对齐（D1死区）
    data.servoData_primary.D2 = 90;
    data.servoData_primary.D3 = 90;
    data.servoData_primary.D4 = 90;
    data.servoData_primary.D5 = 45;
    data.servoData_primary.D6 = 0;

    motorSPEED motorspeed;
    servoANGLE servoangle;
    uint32_t last_dist_ms = 0;
    char dmsg[12];

    while(1)
    {
        uint32_t now = HAL_GetTick();

        //—— 1)超声波非阻塞测距，并每50ms上报一帧 @Dxxxx#（搜索阶段也要持续上报）——
        Ultrasonic_Tick(now, 50);
        if(now - last_dist_ms >= 50){
            last_dist_ms = now;
            int32_t d_cm = Ultrasonic_GetDistance_cm();
            if(d_cm < 0){
                U3_printf((uint8_t*)"@D9999#");     //本次测距无效
            }else{
                if(d_cm > 9998) d_cm = 9998;
                sprintf(dmsg, "@D%04ld#", (long)d_cm);
                U3_printf((uint8_t*)dmsg);
            }
        }

        //—— 2)收 OpenMV 帧（还是调用原来的 readdata4，名字没变）——
        if(rxcplt_flag == 1)
        {
            readdata4(&data, RxData);
            rxcplt_flag = 0;
            ifrxstart = 0;
        }

        //—— 3)退出 ——
        if(data.IFSTOP == 1) {
            U3_printf((uint8_t*)"@ACKST#");
            motor_stop(&motorspeed);
            servo_stop(&servoangle);
            Motor_Sendcmd(&motorspeed);
            Servo_Sendcmd(&servoangle);
            return;
        }

        //—— 4)刷新动作组目录（功能保留，视觉流程默认不发）——
        if(data.IFREFRESH == 1){
            ActionGroup_List_4();
            data.IFREFRESH = 0;
        }

        //—— 5)执行：暂停就停车回中；使能就按视觉帧解算并下发 ——
        if(data.STATUS == 0)
        {
            motor_stop(&motorspeed);
            servo_stop(&servoangle);
            Motor_Sendcmd(&motorspeed);
            Servo_Sendcmd(&servoangle);
        }
        else
        {
            motor_ik(&motorspeed, &data.speedData_primary);
            servo_ik(&servoangle, &data.servoData_primary);
            Motor_Sendcmd(&motorspeed);
            Servo_Sendcmd(&servoangle);
        }
    }
}