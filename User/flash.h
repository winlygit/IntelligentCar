#include "main.h"
#include "global.h"
#include "stm32f1xx_hal.h"
#include <stdint.h>
extern SPI_HandleTypeDef hspi2;
//HIGH IS USEFUL
#define W25Q_SPI            hspi2
#define W25Q_CS_PORT        GPIOB
#define W25Q_CS_PIN         GPIO_PIN_12
#define CS_ENABLE()   HAL_GPIO_WritePin(W25Q_CS_PORT, W25Q_CS_PIN, GPIO_PIN_RESET)
#define CS_DISABLE()  HAL_GPIO_WritePin(W25Q_CS_PORT, W25Q_CS_PIN, GPIO_PIN_SET)  //测试和真实版相反
#define W25Q_CMD_WRITE_ENABLE       0x06
#define W25Q_CMD_READ_STATUS1       0x05
#define W25Q_CMD_READ_DATA          0x03
#define W25Q_CMD_PAGE_PROGRAM       0x02
#define W25Q_CMD_SECTOR_ERASE       0x20
#define W25Q_CMD_BLOCK_ERASE_64K    0xD8
#define W25Q_CMD_CHIP_ERASE         0xC7
#define W25Q_CMD_JEDEC_ID           0x9F
#define W25Q_PAGE_SIZE              256
void W25Q_Init(void);
void W25Q_ReadData(uint32_t addr, uint8_t *pBuf, uint32_t size);
void W25Q_WriteData(uint32_t addr, const uint8_t *pData, uint32_t size);
void W25Q_WritePage(uint32_t addr, const uint8_t *pData, uint16_t len);
void W25Q_ChipErase(void);
void W25Q_SectorErase(uint32_t addr);
void W25Q_WaitBusy(uint32_t timeout_ms);
void W25Q_WriteEnable(void);
uint8_t W25Q_ReadStatusReg1(void);

