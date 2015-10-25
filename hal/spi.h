#ifndef __SPI_H__
#define __SPI_H__

#include <stm32f10x.h>

void spi_init(SPI_TypeDef* SPIx, int prescaler);
uint16_t spi_read(SPI_TypeDef* SPIx);
uint16_t spi_wait_read(SPI_TypeDef* SPIx);
void spi_write(SPI_TypeDef* SPIx, uint16_t data);
void spi_wait_ready(SPI_TypeDef* SPIx);
void spi_send(SPI_TypeDef* SPIx, unsigned char* data, unsigned int length);
void spi_receive(SPI_TypeDef* SPIx, unsigned char* data, unsigned int length);
void spi_send_single(SPI_TypeDef* SPIx, unsigned char data);
unsigned char spi_receive_single(SPI_TypeDef* SPIx);

#endif // __SPI_H__
