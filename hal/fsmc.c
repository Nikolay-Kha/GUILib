#include "fsmc.h"
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_fsmc.h>

// Pointer to write data
#define LCD_DATA    			    (*((volatile uint16_t *)0x60020000))
// Pointer to write command
#define LCD_REG   		  	    (*((volatile uint16_t *)0x60000000))

void HAL_FSMC_Init()
{
	FSMC_NORSRAMInitTypeDef  fsmc;
	FSMC_NORSRAMTimingInitTypeDef fsmcTiming;
	GPIO_InitTypeDef  gpio;

	// Turn on rcc
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE , ENABLE);

	//  FSMC
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);

	// Pins for FSMC
	gpio.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_14 | GPIO_Pin_15;

	gpio.GPIO_Mode = GPIO_Mode_AF_PP;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &gpio);

	gpio.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;

	gpio.GPIO_Mode = GPIO_Mode_AF_PP;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &gpio);

	// CS
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;
	gpio.GPIO_Pin = GPIO_Pin_7;
	GPIO_Init(GPIOD, &gpio);

	// RS
	gpio.GPIO_Pin = GPIO_Pin_11 ;
	GPIO_Init(GPIOD, &gpio);

	// CS -> 1
	// RD -> 1
	// RW -> 1
	GPIO_SetBits(GPIOD, GPIO_Pin_7);
	GPIO_SetBits(GPIOD, GPIO_Pin_4);
	GPIO_SetBits(GPIOD, GPIO_Pin_5);

	// Настройка FSMC
	fsmcTiming.FSMC_AddressSetupTime = 0x02;
	fsmcTiming.FSMC_AddressHoldTime = 0x00;
	fsmcTiming.FSMC_DataSetupTime = 0x05;
	fsmcTiming.FSMC_BusTurnAroundDuration = 0x00;
	fsmcTiming.FSMC_CLKDivision = 0x00;
	fsmcTiming.FSMC_DataLatency = 0x00;
	fsmcTiming.FSMC_AccessMode = FSMC_AccessMode_A;

	fsmc.FSMC_Bank = FSMC_Bank1_NORSRAM1;
	fsmc.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
	fsmc.FSMC_MemoryType = FSMC_MemoryType_SRAM;
	fsmc.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
	fsmc.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
	fsmc.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
	fsmc.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
	fsmc.FSMC_WrapMode = FSMC_WrapMode_Disable;
	fsmc.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
	fsmc.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
	fsmc.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
	fsmc.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
	fsmc.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
	fsmc.FSMC_ReadWriteTimingStruct = &fsmcTiming;
	fsmc.FSMC_WriteTimingStruct = &fsmcTiming;

	FSMC_NORSRAMInit(&fsmc);
	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);
}


/************************************************************************
 **                                                                    **
 ** nCS       ----\__________________________________________/-------  **
 ** RS        ------\____________/-----------------------------------  **
 ** nRD       -------------------------------------------------------  **
 ** nWR       --------\_______/--------\_____/-----------------------  **
 ** DB[0:15]  ---------[index]----------[data]-----------------------  **
 **                                                                    **
 ************************************************************************/
inline void HAL_FSMC_SetRegister(u16 reg)
{
	LCD_REG = reg;
}

inline void HAL_FSMC_WriteData(u16 data)
{
	LCD_DATA = data;
}


//========================================================================
// **                                                                    **
// ** nCS       ----\__________________________________________/-------  **
// ** RS        ------\____________/-----------------------------------  **
// ** nRD       -------------------------\_____/---------------------  **
// ** nWR       --------\_______/--------------------------------------  **
// ** DB[0:15]  ---------[index]----------[data]-----------------------  **
// **                                                                    **
//========================================================================
inline u16 HAL_FSMC_ReadData()
{
	return LCD_DATA;
}

