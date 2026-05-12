
/*
******************************************************************************
        本文件中的函数主要负责解析原始数据
        将所需要的原始数据赋值的到对应的全局变量中
        
        
******************************************************************************
*/




#include "main.h"
#include "readdata.h"
#include "global.h"






void readdata (uint8_t Data[]){              //控制数据帧解析
    
    //电机部分
    
    speedData_primary.Vx = readindexdata4(Data ,2,3,4,5);
    speedData_primary.Vy = -readindexdata4(Data ,7,8,9,10);
    
        if((speedData_primary.Vx <= limit&&speedData_primary.Vx >= -limit)&&(speedData_primary.Vy <= limit&&speedData_primary.Vy >= -limit)){
            speedData_primary.Vx = 0;
            speedData_primary.Vy = 0;
        }
        
        if (Data[WW] == '0'){
            speedData_primary.Wz =   0;
        }else if(Data[WW] == '1'){
            speedData_primary.Wz =  -Vz;
        }else if(Data[WW] == '2'){
            speedData_primary.Wz = Vz;
        }

    //舵机部分
    servoData_primary.D1 = readindexdata3(Data ,18,19,20);
    servoData_primary.D2 = readindexdata4(Data ,35,36,37,38);
    servoData_primary.D3 = readindexdata4(Data ,40,41,42,43);
    servoData_primary.D4 = readindexdata4(Data ,45,46,47,48);
    servoData_primary.D5 = readindexdata3(Data ,23,24,25);
    servoData_primary.D6 = Data[27] - '0';

    //读取动作组
    actionnum = readindexdata3(Data ,31,32,33);
    
        
}


int readindexdata2(uint8_t data[],int index1,int index2){      //读取指定两位数，两位数据，默认为正
    
    
    int a = data[index1] - '0';
    int b = data[index2] - '0';
    int number = a*10 + b*1 ;
    
    return number;
}





int readindexdata3(uint8_t data[],int index1,int index2,int index3){      //读取指定三位数，三位数据，默认为正
    
    
    int a = data[index1] - '0';
    int b = data[index2] - '0';
    int c = data[index3] - '0';
    int number = a*100 + b*10 + c ;
    
    return number;
}


int readindexdata4(uint8_t data[],int index1,int index2,int index3,int index4){      //读取指定四位数，符号+三位数据
    
    char a = data[index1];
    int b = data[index2] - '0';
    int c = data[index3] - '0';
    int d = data[index4] - '0';
    int number = b*100 + c*10 + d ;
    if (a == '-'){
        number = -number;
    }
    return number;
}













