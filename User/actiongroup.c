/*  
本文件用于动作组函数存放，如播放录制整理
                                     */

#include "actiongroup.h"
#include "ag_config.h"
#include "flash.h"
#include "global.h"
#include "motor.h"
#include "servo.h"
#include <string.h>
#include <stdio.h>
#include "uart.h"

/***********全局变量************/

uint16_t error_time = 1;                // 误差补偿，但用RAM缓冲后其实用不上了


volatile int recording_active = 0;      // 表示现在是不是正在录制，1=是，0=不是
uint16_t rec_group_id = 0;              // 当前录制的动作组ID
volatile uint16_t rec_step_count = 0;   // 当前录了多少步了
volatile uint32_t rec_total_time_ms = 0;// 当前录制的总时长


static uint32_t rec_addr;               // 当前录制开始的扇区地址
static char rec_name[16];               // 当前录制的动作组名字
static uint32_t last_rec_tick = 0;      // 上一次采样时的时间戳


static uint8_t sector_buf[SECTOR_SIZE]; // 碎片整理时临时搬数据用的


/* RAM录制缓冲区，大小由MAX_TOTAL_STEPS决定，就是先存这里 */
static ActionStep_t rec_buffer[MAX_TOTAL_STEPS];

// 计算一个动作组要占几个扇区，（向上取整）
uint16_t ActionGroup_SectorCount(ActionGroupHeader_t *hdr)
{
    uint32_t data_size = sizeof(ActionGroupHeader_t) + hdr->step_count * sizeof(ActionStep_t);
    uint16_t sectors = (uint16_t)((data_size + SECTOR_SIZE - 1) / SECTOR_SIZE);
    return sectors;
}


/****************动作组初始化*****************/
void ActionGroup_Init(void)
{
    ActionConfig_Init();    // 初始化配置（读取/重建）
    recording_active = 0;   // 确保不在录制状态
}


/****************动作组录制相关*****************/
int ActionGroup_StartRecord(const char *name, uint8_t name_len)
{
    if (recording_active) {
        return -1; // 已经在录了，不能再开
    }

    // 计算一个动作组最多可能占多少扇区
    uint16_t max_sectors = (uint16_t)((sizeof(ActionGroupHeader_t) +MAX_TOTAL_STEPS * sizeof(ActionStep_t) + SECTOR_SIZE - 1) / SECTOR_SIZE);
    //其实max_sectors = 3，MAX_TOTAL_STEPS = 507，sizeof(ActionStep_t) = 20，sizeof(ActionGroupHeader_t) = 26
    // 获取当前空闲地址
    uint32_t free_addr = ActionConfig_GetFreeSector();

    // 空间不够则先整理碎片
    if (free_addr + max_sectors * SECTOR_SIZE > RECORD_END) {
        ActionGroup_Defrag();
        free_addr = ActionConfig_GetFreeSector();
        if (free_addr + max_sectors * SECTOR_SIZE > RECORD_END) {
            return -1;
        }
    }

    rec_addr = free_addr;
    rec_group_id = ActionConfig_GetNextGroupID(); // 分配ID

    // 清空名字，再拷贝进去
    memset(rec_name, 0, sizeof(rec_name));
    if (name_len > 15) {
        name_len = 15; // 名字最长15个字符
    }
    memcpy(rec_name, name, name_len);

    // 先擦掉后面3个扇区，防止原来有数据影响（3个？
    for (uint16_t s = 0; s < max_sectors; s++) {
        W25Q_SectorErase(rec_addr + s * SECTOR_SIZE);
    }

    rec_step_count = 0;        //步数清零
    rec_total_time_ms = 0;     // 总时间清零
    last_rec_tick = HAL_GetTick(); //   记录一下开始时间
    recording_active = 1;      //设置录制中标志

    return 0;
}

void ActionGroup_StopRecord(void)
{
    if (!recording_active) {
        return; //没在录，直接返回
    }
    recording_active = 0; //关掉标志

    uint16_t steps = rec_step_count; //实际录了多少步

    //计算实际需要几个扇区
    uint16_t sectors_needed = (uint16_t)((sizeof(ActionGroupHeader_t) +
                              steps * sizeof(ActionStep_t) + SECTOR_SIZE - 1) / SECTOR_SIZE);
    if (sectors_needed == 0) {
        sectors_needed = 1; // 至少1个扇区，用来放头部
    }

    // 把除了第一个扇区以外的其他扇区擦掉
    for (uint16_t s = 1; s < sectors_needed; s++) {
        W25Q_SectorErase(rec_addr + s * SECTOR_SIZE);
    }

    //头部信息
    ActionGroupHeader_t hdr;
    memset(&hdr, 0, sizeof(hdr));
    hdr.magic[0] = 0xAA;
    hdr.magic[1] = 0x55;
    hdr.group_id = rec_group_id;
    strncpy(hdr.name, rec_name, 16);
    hdr.step_count = steps;
    hdr.total_time_ms = rec_total_time_ms;

    // 写头部
    W25Q_WriteData(rec_addr, (uint8_t *)&hdr, sizeof(hdr));

    // 如果有步骤数据，就把RAM缓冲区的数据一次性写进去
    if (steps > 0) {
        uint32_t offset = rec_addr + sizeof(ActionGroupHeader_t);
        uint32_t size = steps * sizeof(ActionStep_t);
        W25Q_WriteData(offset, (uint8_t *)rec_buffer, size);
    }

    // 更新配置：空闲地址后移实际使用的扇区数
    uint32_t new_free = rec_addr + sectors_needed * SECTOR_SIZE;
    uint16_t new_id = rec_group_id + 1;
    ActionConfig_Update(new_free, new_id);
}

void ActionGroup_RecordStep(const motorSPEED *motorspeed, const servoANGLE *servoangle, uint8_t d6)
{
    if (!recording_active) return;
    if (rec_step_count >= MAX_TOTAL_STEPS) {
        ActionGroup_StopRecord();
        return;
    }

    ActionStep_t *step = &rec_buffer[rec_step_count];
    step->wheel_fl = motorspeed->LFsd;
    step->wheel_fr = motorspeed->RFsd;
    step->wheel_rl = motorspeed->LBsd;
    step->wheel_rr = motorspeed->RBsd;
    step->joint[0] = servoangle->D1;
    step->joint[1] = servoangle->D2;
    step->joint[2] = servoangle->D3;
    step->joint[3] = servoangle->D4;
    step->joint[4] = servoangle->D5;
    
    step->flags = d6;

    uint32_t now = HAL_GetTick();
    step->duration_ms = now - last_rec_tick;
    last_rec_tick = now;
    rec_step_count++;
    rec_total_time_ms += step->duration_ms;
}

int ActionGroup_IsRecording(void)
{
    return recording_active;
}

/****************动作组查找、删除、列表*****************/
int ActionGroup_Find(uint16_t id, uint32_t *addr)
{
    uint32_t scan = 0;
    uint32_t tail = ActionConfig_GetFreeSector();

    while (scan < tail) {
        uint8_t magic[2];
        W25Q_ReadData(scan, magic, 2);

        if (magic[0] == 0xAA && magic[1] == 0x55) {
            ActionGroupHeader_t hdr;
            W25Q_ReadData(scan, (uint8_t *)&hdr, sizeof(hdr));

            if (hdr.group_id == id) {
                *addr = scan;
                return 1;
            }

            uint16_t sectors = ActionGroup_SectorCount(&hdr);
            scan += sectors * SECTOR_SIZE;
        } else {
            scan += SECTOR_SIZE;
        }
    }

    return 0;
}

int ActionGroup_Delete(uint16_t id)
{
    uint32_t addr;

    if (ActionGroup_Find(id, &addr)) {
        ActionGroupHeader_t hdr;
        W25Q_ReadData(addr, (uint8_t *)&hdr, sizeof(hdr));
        uint16_t sectors = ActionGroup_SectorCount(&hdr);

        for (uint16_t s = 0; s < sectors; s++) {
            W25Q_SectorErase(addr + s * SECTOR_SIZE);
        }

        ActionGroup_Defrag(); // 删除后整理
        return 1;
    }

    return 0;
}

void ActionGroup_List(void)
{
    uint8_t list_buf[512];      // 内部缓冲区，存放二进制列表数据
    uint16_t list_len = 0;      // 实际写入的字节数

    uint16_t count = 0;
    uint32_t scan = 0;
    uint32_t tail = ActionConfig_GetFreeSector();

    uint8_t *p = list_buf + 1;
    uint16_t remaining = sizeof(list_buf) - 1;

    while (scan < tail && remaining >= 24) {
        uint8_t magic[2];
        W25Q_ReadData(scan, magic, 2);

        if (magic[0] == 0xAA && magic[1] == 0x55) {
            ActionGroupHeader_t hdr;
            W25Q_ReadData(scan, (uint8_t *)&hdr, sizeof(hdr));

            // 填充动作组信息：编号(2B)+步数(2B)+总时间(4B)+名称(16B)
            *p++ = hdr.group_id & 0xFF;
            *p++ = (hdr.group_id >> 8) & 0xFF;
            *p++ = hdr.step_count & 0xFF;
            *p++ = (hdr.step_count >> 8) & 0xFF;
            *p++ = hdr.total_time_ms & 0xFF;
            *p++ = (hdr.total_time_ms >> 8) & 0xFF;
            *p++ = (hdr.total_time_ms >> 16) & 0xFF;
            *p++ = (hdr.total_time_ms >> 24) & 0xFF;

            for (int i = 0; i < 16; i++) {
                *p++ = hdr.name[i];
            }

            count++;
            remaining -= 24;

            uint16_t sectors = ActionGroup_SectorCount(&hdr);
            scan += sectors * SECTOR_SIZE;
        } else {
            scan += SECTOR_SIZE;
        }
    }
    list_buf[0] = count;                 // 第一个字节存放数量
    list_len = (uint16_t)(p - list_buf); // 实际使用长度

    //发送逻辑
    if (list_len < 1) {
        U3_printf((uint8_t*)"Total:0\n");
        return;
    }

    char msg[96];
    sprintf(msg, "Total:%d\n", count);
    U3_printf((uint8_t*)msg);

    uint8_t *ptr = &list_buf[1];
    for (uint8_t i = 0; i < count; i++) {
        if (ptr + 24 > list_buf + list_len) {
            break;
        }

        uint16_t group_id   = ptr[0] | (ptr[1] << 8);
        uint16_t step_count = ptr[2] | (ptr[3] << 8);
        uint32_t total_time = ptr[4] | (ptr[5] << 8) | (ptr[6] << 16) | ((uint32_t)ptr[7] << 24);

        char name[17];
        memcpy(name, &ptr[8], 16);
        name[16] = '\0';

        sprintf(msg, "ID:%u,Steps:%u,Time:%lu,Name:%s\n",group_id, step_count, (unsigned long)total_time, name);
        U3_printf((uint8_t*)msg);

        ptr += 24;
    }
}

/****************碎片整理*****************/
void ActionGroup_Defrag(void)
{
    uint32_t read_addr = 0;
    uint32_t write_addr = 0;
    uint32_t tail = ActionConfig_GetFreeSector();

    while (read_addr < tail) {
        uint8_t magic[2];
        W25Q_ReadData(read_addr, magic, 2);

        if (magic[0] != 0xAA || magic[1] != 0x55) {
            read_addr += SECTOR_SIZE;
            continue;
        }

        ActionGroupHeader_t hdr;
        W25Q_ReadData(read_addr, (uint8_t *)&hdr, sizeof(hdr));
        uint16_t sectors = ActionGroup_SectorCount(&hdr);

        if (read_addr != write_addr) {
            for (uint16_t s = 0; s < sectors; s++) {
                uint32_t src = read_addr + s * SECTOR_SIZE;
                uint32_t dst = write_addr + s * SECTOR_SIZE;

                W25Q_ReadData(src, sector_buf, SECTOR_SIZE);
                W25Q_SectorErase(dst);
                W25Q_WriteData(dst, sector_buf, SECTOR_SIZE);
                W25Q_SectorErase(src);
            }
        }

        read_addr += sectors * SECTOR_SIZE;
        write_addr += sectors * SECTOR_SIZE;
    }

    // 更新配置中的空闲扇区地址，ID保持不变
    ActionConfig_Update(write_addr, ActionConfig_GetNextGroupID());
}

/****************播放*****************/

void ActionGroup_Play(uint32_t addr, motorSPEED *motorspeed, servoANGLE *servoangle)
{
    ActionGroupHeader_t hdr;
    W25Q_ReadData(addr, (uint8_t*)&hdr, sizeof(hdr));
    if (hdr.magic[0] != 0xAA || hdr.magic[1] != 0x55) return;

    for (uint16_t i = 0; i < hdr.step_count; i++) {
        uint32_t step_start = HAL_GetTick();
        ActionStep_t step;
        W25Q_ReadData(addr + sizeof(ActionGroupHeader_t) + i * sizeof(ActionStep_t),
                      (uint8_t*)&step, sizeof(step));

        motorspeed->LFsd = step.wheel_fl;
        motorspeed->RFsd = step.wheel_fr;
        motorspeed->LBsd = step.wheel_rl;
        motorspeed->RBsd = step.wheel_rr;
        servoangle->D1 = step.joint[0];
        servoangle->D2 = step.joint[1];
        servoangle->D3 = step.joint[2];
        servoangle->D4 = step.joint[3];
        servoangle->D5 = step.joint[4];
        servoangle->D6 = (step.flags == 1) ? 140 : 68;

        Motor_Sendcmd(motorspeed);
        Servo_Sendcmd(servoangle);

        uint32_t elapsed = HAL_GetTick() - step_start;
        if (elapsed < step.duration_ms) {
            HAL_Delay(step.duration_ms - elapsed);
        }
    }
}