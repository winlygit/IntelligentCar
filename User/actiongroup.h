#ifndef __ACTIONGROUP_H
#define __ACTIONGROUP_H
#include <stdint.h>
#include "global.h"

#define SECTOR_SIZE             4096
#define RECORD_END              0x7FE000   // 录制数据结束地址(**是配置扇区的起始地址)

/* 每个扇区最多可容纳的步骤数 */
#define MAX_STEPS_PER_SECTOR    260

// 后续根据RAM缓冲区大小实测
#define MAX_RECORD_SECTORS      3

#define MAX_TOTAL_STEPS         (MAX_STEPS_PER_SECTOR * MAX_RECORD_SECTORS)

// 配置扇区：使用最后两个扇区，双备份
#define CONFIG_SECTOR_A         0x7FE000
#define CONFIG_SECTOR_B         0x7FF000


/* 结构体 */
#pragma pack(push, 1)

// 每个动作步骤的结构体
typedef struct {
    uint32_t duration_ms;       // 本步骤持续时间 (ms)
    int16_t  joint[5];          // 5个关节角度 (0.1°)
    uint8_t  flags;             // 特殊标志（如夹爪）
} ActionStep_t;

// 动作组头部结构体
typedef struct {
    uint8_t  magic[2];          // 魔数 0xAA 0x55
    uint16_t group_id;          // 动作组编号
    char     name[16];          // 名称
    uint16_t step_count;        // 实际步骤数
    uint32_t total_time_ms;     // 总时长 (ms)，最长约39s
} ActionGroupHeader_t;
#pragma pack(pop)


// config结构体
typedef struct {
    uint32_t tail_free_sector;  // 尾部空闲扇区地址（4KB对齐）
    uint16_t next_group_id;     // 下一个可分配的动作组编号
    uint8_t  reserved[2];
    uint32_t version;           // 版本号，用于双备份选择
    uint32_t checksum;          // 校验和
} SystemConfig_t;


extern volatile int recording_active;   // 1=正在录制
extern uint16_t rec_group_id;           // 当前录制组编号
extern volatile uint16_t rec_step_count;// 当前录制步骤数
extern volatile uint32_t rec_total_time_ms;// 当前录制总时长


void ActionGroup_Init(void);
int ActionGroup_StartRecord(const char *name, uint8_t name_len);
void ActionGroup_StopRecord(void);
int ActionGroup_IsRecording(void);
int ActionGroup_Find(uint16_t id, uint32_t *addr);
int ActionGroup_Delete(uint16_t id);
void ActionGroup_List_3(void);
void ActionGroup_List_4(void);
void ActionGroup_Defrag(void);
void ActionGroup_RecordStep(const servoANGLE *servoangle, uint8_t d6);
void ActionGroup_Play(uint32_t addr, servoANGLE *servoangle);


uint16_t ActionGroup_SectorCount(ActionGroupHeader_t *hdr);

#endif /* __ACTIONGROUP_H */

