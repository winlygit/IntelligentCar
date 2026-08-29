
#include "ag_config.h"
#include "actiongroup.h"
#include "flash.h"
#include <string.h>

static SystemConfig_t sys_cfg;

// 计算配置校验和（前12字节累加，不包含checksum本身）
static uint32_t calc_checksum(SystemConfig_t *cfg)
{
    uint8_t *p = (uint8_t *)cfg;
    uint32_t sum = 0;
    for (int i = 0; i < 12; i++) {
        sum += p[i];
    }
    return sum;
}


/************校验配置有效性************/
static int is_config_valid(SystemConfig_t *cfg)
{
    if (cfg->checksum != calc_checksum(cfg)) return 0;
    if (cfg->tail_free_sector % SECTOR_SIZE != 0) return 0;
    if (cfg->tail_free_sector > RECORD_END) return 0;
    return 1;
}

/* 保存配置到A/B两个备份区 */
static void save_config(void)
{
    sys_cfg.version = sys_cfg.version + 1;
    sys_cfg.checksum = calc_checksum(&sys_cfg);

    W25Q_SectorErase(CONFIG_SECTOR_A);
    W25Q_WriteData(CONFIG_SECTOR_A, (uint8_t *)&sys_cfg, sizeof(SystemConfig_t));

    W25Q_SectorErase(CONFIG_SECTOR_B);
    W25Q_WriteData(CONFIG_SECTOR_B, (uint8_t *)&sys_cfg, sizeof(SystemConfig_t));
}

//扫描整个flash，重建配置（用于配置损坏时）
static void scan_and_rebuild_config(void)
{
    uint32_t addr = 0;
    uint16_t max_id = 0;

    sys_cfg.tail_free_sector = 0;
    sys_cfg.next_group_id = 1;

    while (addr < RECORD_END) {
        uint8_t magic[2];
        W25Q_ReadData(addr, magic, 2);

        //检查是否为全0xFF（空闲扇区），若是则停止扫描
        if (magic[0] == 0xFF && magic[1] == 0xFF) {
            break;  //到达空闲区，后续无需继续
        }

        if (magic[0] == 0xAA && magic[1] == 0x55) {
            uint16_t id;
            W25Q_ReadData(addr + 2, (uint8_t *)&id, 2);
            if (id > max_id) max_id = id;

            ActionGroupHeader_t hdr;
            W25Q_ReadData(addr, (uint8_t *)&hdr, sizeof(hdr));
            uint16_t sectors = ActionGroup_SectorCount(&hdr);
            addr += sectors * SECTOR_SIZE;
        } else {
            //既不是有效动作组，也不是全FF的空闲扇区，说明数据损坏，
            addr += SECTOR_SIZE;
        }
    }

    sys_cfg.tail_free_sector = addr;
    sys_cfg.next_group_id = max_id + 1;
    sys_cfg.version = 0;
    save_config();
}

/* 配置初始化：读取A/B区，选择有效且版本新者 */
void ActionConfig_Init(void)
{
    SystemConfig_t cfgA, cfgB;

    W25Q_ReadData(CONFIG_SECTOR_A, (uint8_t *)&cfgA, sizeof(SystemConfig_t));
    W25Q_ReadData(CONFIG_SECTOR_B, (uint8_t *)&cfgB, sizeof(SystemConfig_t));

    int va = is_config_valid(&cfgA) ? (int)cfgA.version : -1;
    int vb = is_config_valid(&cfgB) ? (int)cfgB.version : -1;

    if (va >= 0 || vb >= 0) {
        if(va > vb)      		sys_cfg = cfgA;
        else if(vb > va)	  sys_cfg = cfgB;
        else            	  sys_cfg = cfgA;   // 相同则用A
    } else {
        scan_and_rebuild_config();
    }
}

//获取当前空闲扇区地址
uint32_t ActionConfig_GetFreeSector(void)
{
    return sys_cfg.tail_free_sector;
}

//得到下一个可分配的动作组ID
uint16_t ActionConfig_GetNextGroupID(void)
{
    return sys_cfg.next_group_id;
}

//手动更新空闲扇区和下一个ID
void ActionConfig_Update(uint32_t new_free_sector, uint16_t new_next_id)
{
    sys_cfg.tail_free_sector = new_free_sector;
    sys_cfg.next_group_id = new_next_id;
    save_config();
}

