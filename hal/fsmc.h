#ifndef _HAL_FSMC_H_
#define _HAL_FSMC_H_

#include <stm32f10x.h>

void HAL_FSMC_Init();
void HAL_FSMC_SetRegister(u16 reg);
void HAL_FSMC_WriteData(u16 data);
u16 HAL_FSMC_ReadData();

#endif //_HAL_FSMC_H_
