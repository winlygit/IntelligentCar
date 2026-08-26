
/*
******************************************************************************
        本文件中的函数主要负责解析原始数据
        将所需要的原始数据赋值的到对应的全局变量中
        
        
******************************************************************************
*/




#include "main.h"
#include "readdata.h"
#include "global.h"

int readindexdata2(uint8_t *data,int index1,int index2){      //读取指定两位数，两位数据，默认为正
    
    
    int a = data[index1] - '0';
    int b = data[index2] - '0';
    int number = a*10 + b*1 ;
    
    return number;
}





int readindexdata3(uint8_t *data,int index1,int index2,int index3){      //读取指定三位数，三位数据，默认为正
    
    
    int a = data[index1] - '0';
    int b = data[index2] - '0';
    int c = data[index3] - '0';
    int number = a*100 + b*10 + c ;
    
    return number;
}


int readindexdata4(uint8_t *data,int index1,int index2,int index3,int index4){      //读取指定四位数，符号+三位数据
    
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




MODE readmode (uint8_t *Data){              //模式解析函数

    //解析模式部分
    MODE mode;
    if(Data[1] == 'M' && Data[2] == '1'){
        mode = mode1;
    }else if(Data[1] == 'M' && Data[2] == '2'){
        mode = mode2;
    }else if(Data[1] == 'M' && Data[2] == '3'){
        mode = mode3;
    }else if(Data[1] == 'M' && Data[2] == '4'){
        mode = mode4;
    }else{
        mode = STOP;
    }
    return mode;
}

void readdata1(mode1_data *data,uint8_t* Data)              //模式1数据解析函数
{
    if(Data[1]=='S'&&Data[2]=='T'){
        data->IFSTOP = 1;//需手动归零
    }else if(Data[3]== 'A'&&Data[4]== 'C'){
        data->STATUS = 1;
    }else if(Data[3]== 'S'&&Data[4]== 'T'){
        data->STATUS = 0;
    }else if(Data[3]== ','){
    //电机部分
    
    data->speedData_primary.Vx = readindexdata4(Data ,4,5,6,7);
    data->speedData_primary.Vy = -readindexdata4(Data ,9,10,11,12);
    
    if((data->speedData_primary.Vx <= limit&&data->speedData_primary.Vx >= -limit)&&(data->speedData_primary.Vy <= limit&&data->speedData_primary.Vy >= -limit)){
        data->speedData_primary.Vx = 0;
        data->speedData_primary.Vy = 0;
    }
    #define WW 29     //控制wz的数据的位置
    if (Data[WW] == '0'){
        data->speedData_primary.Wz =   0;
    }else if(Data[WW] == '1'){
        data->speedData_primary.Wz =  -Vz;
    }else if(Data[WW] == '2'){
        data->speedData_primary.Wz = Vz;
    }

    //舵机部分
    data->servoData_primary.D1 = readindexdata3(Data ,19,20,21);
    data->servoData_primary.D2 = readindexdata4(Data ,31,32,33,34);
    data->servoData_primary.D3 = readindexdata4(Data ,36,37,38,39);
    data->servoData_primary.D4 = readindexdata4(Data ,41,42,43,44);
    data->servoData_primary.D5 = readindexdata3(Data ,23,24,25);
    data->servoData_primary.D6 = Data[27] - '0';
    }

}

void readdata2(mode2_data *data,uint8_t* Data)              //模式2数据解析函数
{
    if(Data[1]=='S'&&Data[2]=='T'){
        data->IFSTOP = 1;//需手动归零
    }else if(Data[3]== 'A'&&Data[4]== 'C'){
        data->STATUS = 1;
        data->NAME[0] = Data[9];
        data->NAME[1] = Data[10];
        data->NAME[2] = Data[11];
    }else if(Data[3]== 'S'&&Data[4]== 'T'){
        data->STATUS = 0;
    }else if(Data[3]== ','){
    //电机部分
    
    data->speedData_primary.Vx = readindexdata4(Data ,4,5,6,7);
    data->speedData_primary.Vy = -readindexdata4(Data ,9,10,11,12);
    
    if((data->speedData_primary.Vx <= limit&&data->speedData_primary.Vx >= -limit)&&(data->speedData_primary.Vy <= limit&&data->speedData_primary.Vy >= -limit)){
        data->speedData_primary.Vx = 0;
        data->speedData_primary.Vy = 0;
    }
    #define WW 29     //控制wz的数据的位置
    if (Data[WW] == '0'){
        data->speedData_primary.Wz =   0;
    }else if(Data[WW] == '1'){
        data->speedData_primary.Wz =  -Vz;
    }else if(Data[WW] == '2'){
        data->speedData_primary.Wz = Vz;
    }

    //舵机部分
    data->servoData_primary.D1 = readindexdata3(Data ,19,20,21);
    data->servoData_primary.D2 = readindexdata4(Data ,31,32,33,34);
    data->servoData_primary.D3 = readindexdata4(Data ,36,37,38,39);
    data->servoData_primary.D4 = readindexdata4(Data ,41,42,43,44);
    data->servoData_primary.D5 = readindexdata3(Data ,23,24,25);
    data->servoData_primary.D6 = Data[27] - '0';
    }

}

void readdata3(mode3_data *data,uint8_t* Data)              //模式3数据解析函数
{
    if(Data[1]=='S'&&Data[2]=='T'){
        data->IFSTOP = 1;//需手动归零
    }else if(Data[3]== 'R'&&Data[4]== 'E'){
        data->IFREFRESH = 1;//需手动归零
    }else if(Data[3]== 'B'){
        data->IFEXECUTE = 1;//需手动归零
        data->ACTIONID = readindexdata3(Data ,4,5,6);
    }else if(Data[3]== 'D'){
        data->IFDELETE = 1;//需手动归零
        data->DELETEID = readindexdata3(Data ,4,5,6);
    }
}

void readdata4(mode4_data *data,uint8_t* Data)              //模式4数据解析函数
{
    if(Data[1]=='S'&&Data[2]=='T'){
        data->IFSTOP = 1;//需手动归零
    }else if(Data[3]== 'R'&&Data[4]== 'E'){
        data->IFREFRESH = 1;//需手动归零
    }else if(Data[3]== 'A'&&Data[4]== 'C'&&Data[5]== 'L'){
        data->STATUS = 1;
        data->ACTIONID = readindexdata3(Data ,6,7,8);
        data->DISTANCE = readindexdata2(Data ,10,11);
    }else if(Data[3]== 'S'&&Data[4]== 'T'){
        data->STATUS = 0;
    }

}

int IFREACH_check(mode4_data *data, int16_t distance) {
    if (distance <= data->DISTANCE) {
        data->IFREACH = 1;
        return 1; // 达到距离阈值
    } else {
        data->IFREACH = 0;
        return 0; // 未达到距离阈值
    }
}
    // //电机部分
    
    // speedData_primary.Vx = readindexdata4(Data ,2,3,4,5);
    // speedData_primary.Vy = -readindexdata4(Data ,7,8,9,10);
    
    //     if((speedData_primary.Vx <= limit&&speedData_primary.Vx >= -limit)&&(speedData_primary.Vy <= limit&&speedData_primary.Vy >= -limit)){
    //         speedData_primary.Vx = 0;
    //         speedData_primary.Vy = 0;
    //     }
        
    //     if (Data[WW] == '0'){
    //         speedData_primary.Wz =   0;
    //     }else if(Data[WW] == '1'){
    //         speedData_primary.Wz =  -Vz;
    //     }else if(Data[WW] == '2'){
    //         speedData_primary.Wz = Vz;
    //     }

    // //舵机部分
    // servoData_primary.D1 = readindexdata3(Data ,18,19,20);
    // servoData_primary.D2 = readindexdata4(Data ,35,36,37,38);
    // servoData_primary.D3 = readindexdata4(Data ,40,41,42,43);
    // servoData_primary.D4 = readindexdata4(Data ,45,46,47,48);
    // servoData_primary.D5 = readindexdata3(Data ,23,24,25);
    // servoData_primary.D6 = Data[27] - '0';

    // //读取动作组
    // actionnum = readindexdata3(Data ,31,32,33);
        
















