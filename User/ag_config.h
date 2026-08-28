#ifndef __AG_CONFIG_H
#define __AG_CONFIG_H

#include <stdint.h>


void     ActionConfig_Init(void);
uint32_t ActionConfig_GetFreeSector(void);
uint16_t ActionConfig_GetNextGroupID(void);
void     ActionConfig_Update(uint32_t new_free_sector, uint16_t new_next_id);

#endif /* __ACTION_CONFIG_H */

