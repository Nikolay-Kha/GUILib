#include "spi.h"
#include <stm32f10x.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_spi.h>
#include <stm32f10x_rcc.h>

void spi_init(SPI_TypeDef* SPIx, int prescaler)
{
	assert_param(IS_SPI_ALL_PERIPH(SPIx));
	if(SPIx==SPI1) {
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

		GPIO_InitTypeDef gpio;
		GPIO_StructInit(&gpio);
		gpio.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
		gpio.GPIO_Speed = GPIO_Speed_50MHz;
		gpio.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_Init(GPIOA, &gpio);
		gpio.GPIO_Pin = GPIO_Pin_6;
		gpio.GPIO_Mode = GPIO_Mode_IPU;
		GPIO_Init(GPIOA, &gpio);
	} else if(SPIx==SPI2) {
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

		GPIO_InitTypeDef gpio;
		GPIO_StructInit(&gpio);
		gpio.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15;
		gpio.GPIO_Speed = GPIO_Speed_50MHz;
		gpio.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_Init(GPIOB, &gpio);
		gpio.GPIO_Pin = GPIO_Pin_14;
		gpio.GPIO_Mode = GPIO_Mode_IPU;
		GPIO_Init(GPIOB, &gpio);
	} else if(SPIx==SPI3) {
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);

		GPIO_InitTypeDef gpio;
		GPIO_StructInit(&gpio);
		gpio.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_5;
		gpio.GPIO_Speed = GPIO_Speed_50MHz;
		gpio.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_Init(GPIOB, &gpio);
		gpio.GPIO_Pin = GPIO_Pin_4;
		gpio.GPIO_Mode = GPIO_Mode_IPU;
		GPIO_Init(GPIOB, &gpio);
	}

    SPI_InitTypeDef spi;
    SPI_StructInit(&spi);
	spi.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	spi.SPI_Mode = SPI_Mode_Master;
	spi.SPI_DataSize = SPI_DataSize_8b;
	spi.SPI_CPOL = SPI_CPOL_Low;
	spi.SPI_CPHA = SPI_CPHA_1Edge;
	spi.SPI_NSS = SPI_NSS_Soft;
	switch(prescaler) {
	case 2:
		spi.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
		break;
	case 4:
		spi.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
		break;
	case 8:
		spi.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
		break;
	case 16:
		spi.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
		break;
	case 32:
		spi.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
		break;
	case 64:
		spi.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;
		break;
	case 128:
		spi.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128;
		break;
	case 256:
		spi.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
		break;
	default:
		spi.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
	}
	spi.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_Init(SPIx, &spi);
	SPI_Cmd(SPIx, ENABLE);
	SPI_NSSInternalSoftwareConfig(SPIx, SPI_NSSInternalSoft_Set);
}

uint16_t spi_read(SPI_TypeDef* SPIx)
{
	return SPI_I2S_ReceiveData(SPIx);
}

uint16_t spi_wait_read(SPI_TypeDef* SPIx)
{
	while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE)==RESET);
	return spi_read(SPIx);
}

void spi_write(SPI_TypeDef* SPIx, uint16_t data)
{
	while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE)==RESET);
	SPI_I2S_SendData(SPIx, data);
}

void spi_wait_ready(SPI_TypeDef* SPIx)
{
	while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_BSY) == SET);
}

void spi_send(SPI_TypeDef* SPIx, unsigned char* data, unsigned int length)
{
	while (length--)
	{
		spi_send_single(SPIx, *data);
		data++;
	}
}

void spi_receive(SPI_TypeDef* SPIx, unsigned char* data, unsigned int length)
{
	while (length--)
	{
		*data = spi_receive_single(SPIx);
		data++;
	}
}

void spi_send_single(SPI_TypeDef* SPIx, unsigned char data)
{
	SPIx->DR = data; // write data to be transmitted to the SPI data register
	while( !(SPIx->SR & SPI_I2S_FLAG_TXE) ); // wait until transmit complete
	while( !(SPIx->SR & SPI_I2S_FLAG_RXNE) ); // wait until receive complete
	while( SPIx->SR & SPI_I2S_FLAG_BSY ); // wait until SPI is not busy anymore
	data = SPIx->DR; // return received data from SPI data register
}

unsigned char spi_receive_single(SPI_TypeDef* SPIx)
{
	SPIx->DR = 0xFF; // write data to be transmitted to the SPI data register
	while( !(SPIx->SR & SPI_I2S_FLAG_TXE) ); // wait until transmit complete
	while( !(SPIx->SR & SPI_I2S_FLAG_RXNE) ); // wait until receive complete
	while( SPIx->SR & SPI_I2S_FLAG_BSY ); // wait until SPI is not busy anymore
	return SPIx->DR; // return received data from SPI data register
}
