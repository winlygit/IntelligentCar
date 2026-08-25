//  本文件是W25Q64芯片的驱动

#include "flash.h"
#include "main.h"
#include <stdio.h>
#include <stdint.h>

//SPI封装函数
static void SPI_SendByte(uint8_t byte)
{
    HAL_SPI_Transmit(&hspi2, &byte, 1, HAL_MAX_DELAY);
}
static uint8_t SPI_RecvByte(void)
{
    uint8_t byte;
    HAL_SPI_Receive(&hspi2, &byte, 1, HAL_MAX_DELAY);
    return byte;
}
static void SPI_WriteBuffer(const uint8_t *pData, uint16_t len)
{
    HAL_SPI_Transmit(&hspi2, (uint8_t *)pData, len, HAL_MAX_DELAY);
}
static void SPI_ReadBuffer(uint8_t *pBuf, uint16_t len)
{
    HAL_SPI_Receive(&hspi2, pBuf, len, HAL_MAX_DELAY);
}


void W25Q_Init(void)
{
    CS_DISABLE();
}

uint32_t W25Q_ReadID(void)
{
    uint8_t cmd = W25Q_CMD_JEDEC_ID;
    uint8_t id[3] = {0};

    CS_ENABLE();
    SPI_SendByte(cmd);
    SPI_ReadBuffer(id, 3);
    CS_DISABLE();

    return ((uint32_t)id[0] << 16) | ((uint32_t)id[1] << 8) | id[2];
}

uint8_t W25Q_ReadStatusReg1(void)
{
    uint8_t cmd = W25Q_CMD_READ_STATUS1;
    uint8_t status;

    CS_ENABLE();
    SPI_SendByte(cmd);
    status = SPI_RecvByte();
    CS_DISABLE();

    return status;
}

void W25Q_WriteEnable(void)
{
    uint8_t cmd = W25Q_CMD_WRITE_ENABLE;
    CS_ENABLE();
    SPI_SendByte(cmd);
    CS_DISABLE();
}

void W25Q_WaitBusy(uint32_t timeout_ms)
{
    uint8_t status;
    uint32_t tickstart = HAL_GetTick();

    do {
        status = W25Q_ReadStatusReg1();
        if ((HAL_GetTick() - tickstart) >= timeout_ms) {
            break;   // 超时退出
        }
    } while (status & 0x01);
}

//块擦除数据
void W25Q_SectorErase(uint32_t addr)
{
    uint8_t cmd[4] = {
        W25Q_CMD_SECTOR_ERASE,
        (uint8_t)(addr >> 16),
        (uint8_t)(addr >> 8),
        (uint8_t)(addr)
    };
    W25Q_WriteEnable();
    CS_ENABLE();
    SPI_WriteBuffer(cmd, 4);
    CS_DISABLE();
    W25Q_WaitBusy(1000);
}

//调用一次，清除所有测试数据
void W25Q_ChipErase(void)
{
    uint8_t cmd = W25Q_CMD_CHIP_ERASE;
    W25Q_WriteEnable();
    CS_ENABLE();
    SPI_SendByte(cmd);
    CS_DISABLE();
    W25Q_WaitBusy(20000);
}

//页写入
void W25Q_WritePage(uint32_t addr, const uint8_t *pData, uint16_t len)
{
    if (len == 0) return;
    if (len > 256) len = 256;   // 安全截断

    W25Q_WriteEnable();
    uint8_t cmd[4] = {
        0x02,
        (uint8_t)(addr >> 16),
        (uint8_t)(addr >> 8),
        (uint8_t)(addr)
    };
    CS_ENABLE();
    HAL_SPI_Transmit(&hspi2, cmd, 4, HAL_MAX_DELAY);
    HAL_SPI_Transmit(&hspi2, (uint8_t*)pData, len, HAL_MAX_DELAY);
    CS_DISABLE();
    W25Q_WaitBusy(100);         // 安全超时
}

//任意位置写入
void W25Q_WriteData(uint32_t addr, const uint8_t *pData, uint32_t size)
{
    uint32_t remain = size;
    uint32_t offset = addr;

    while (remain > 0) {
        uint32_t page_offset = offset % W25Q_PAGE_SIZE;
        uint32_t write_len = W25Q_PAGE_SIZE - page_offset;
        if (write_len > remain) write_len = remain;

        W25Q_WritePage(offset, pData, (uint16_t)write_len);

        pData  += write_len;
        offset += write_len;
        remain -= write_len;
    }
}

void W25Q_ReadData(uint32_t addr, uint8_t *pBuf, uint32_t size)
{
    uint8_t cmd[4] = {
        W25Q_CMD_READ_DATA,
        (uint8_t)(addr >> 16),
        (uint8_t)(addr >> 8),
        (uint8_t)(addr)
    };
    CS_ENABLE();
    SPI_WriteBuffer(cmd, 4);
    SPI_ReadBuffer(pBuf, (uint16_t)size);
    CS_DISABLE();
}


