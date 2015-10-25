#include "halProxy.h"
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>

/******************************************************
REIMPLEMENTS FUNCTION IN THIS FILE FOR YOUR LCD DRIVER
******************************************************/
#include "spi.h"

#define LCD_EXTRA_PORT GPIOB
#define LCD_PIN_CS GPIO_Pin_11
#define LCD_PIN_DC GPIO_Pin_12

void LCD_HAL_Init() {
	spi_init(SPI2, 2);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitTypeDef  gpio;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio.GPIO_Pin = LCD_PIN_CS | LCD_PIN_DC; // 11 - CS, 12 - D/C
	GPIO_Init(LCD_EXTRA_PORT, &gpio);
}

void LCD_HAL_Reset() {
	// software emultaion for ili9341
	LCD_HAL_SetRegister(0x01);
	int i;
	for(i=0; i<0x00FFF; i++);
}

void LCD_HAL_Light(int level) {
	// do nothing
}

inline void LCD_HAL_SetRegister(u16 reg) {
	spi_wait_ready(SPI2);
	LCD_EXTRA_PORT->BRR =  LCD_PIN_DC; // DC=0
	LCD_EXTRA_PORT->BRR =  LCD_PIN_CS; // CS=0
	spi_write(SPI2, reg);
}

inline void LCD_HAL_WriteData(u16 data) {
	spi_wait_ready(SPI2);
	LCD_EXTRA_PORT->BSRR =  LCD_PIN_DC; // DC=1
	LCD_EXTRA_PORT->BRR =  LCD_PIN_CS; // CS=0
	spi_write(SPI2, data);
}

inline u16 LCD_HAL_ReadData() {
	spi_wait_ready(SPI2);
	LCD_EXTRA_PORT->BSRR =  LCD_PIN_DC; // DC=1
	LCD_EXTRA_PORT->BRR =  LCD_PIN_CS; // CS=0
	return spi_read(SPI2);
}

inline u16 LCD_HAL_ReadRegister(u16 reg) {
	LCD_HAL_SetRegister(reg);
	return LCD_HAL_ReadData();
}

/******************************************************
REIMPLEMENTS FUNCTION IN THIS FILE FOR YOUR TOUCH DRIVER
******************************************************/
SPI_TypeDef*  TS_HAL_Init() {
	// do nothing

	return 0;
}

/*void EXTI9_5_IRQHandler (void) { // touch itteruption handler
	if (EXTI->PR & (1<<6)) // Прерывание от EXTI6?
	{
		EXTI->PR |= (1<<6);
		TS_TouchIrq();
	}
}*/

inline void TS_HAL_CS() {
	// do nothing
}

inline void TS_HAL_DCS() {
	// do nothing
}

uint32_t TS_HAL_Sense() {
	return 0; // do nothing
}

