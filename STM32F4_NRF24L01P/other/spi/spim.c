#include <stm32f4xx_gpio.h>
#include <stm32f4xx_rcc.h>
#include "spim.h"

//==============================================================================
// Процедура инициализации spi в режиме master
//==============================================================================
void spim_init(SPI_TypeDef* SPIx, uint8_t WordLen)
{
	// Стартуем тактирование GPIO и SPI
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
	// RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_SPI3);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_SPI3);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_SPI3);

	// Настраиваем ноги GPIO для сигналов SCK и MOSI
	GPIO_InitTypeDef InitStruct;
	InitStruct.GPIO_Mode = GPIO_Mode_AF;
	InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	InitStruct.GPIO_OType = GPIO_OType_PP;
	InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;

	// SCK-Pin
	InitStruct.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOB, &InitStruct);
	// MOSI-Pin
	InitStruct.GPIO_Pin = GPIO_Pin_4;
	GPIO_Init(GPIOB, &InitStruct);
	// MISO-Pin
	InitStruct.GPIO_Pin = GPIO_Pin_12;
	GPIO_Init(GPIOC, &InitStruct);

	// Настраиваем SPI
	SPI_Cmd(SPIx, DISABLE);
	SPI_InitTypeDef SPI_InitStruct;

	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex; // SPI_Direction_1Line_Tx;
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
	SPI_InitStruct.SPI_DataSize = (WordLen == 16) ? SPI_DataSize_16b : SPI_DataSize_8b;
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStruct.SPI_CRCPolynomial = 0x7;
	SPI_Init(SPIx, &SPI_InitStruct);
	SPI_Cmd(SPIx, ENABLE);

	SPI_NSSInternalSoftwareConfig(SPIx, SPI_NSSInternalSoft_Set);
}
//==============================================================================

//==============================================================================
uint8_t hal_nrf_rw(uint8_t data)
{
	/* Loop while DR register in not emplty */
	while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == RESET);

	/* Send a Byte through the SPI peripheral */
	SPI_I2S_SendData(SPI3, data);

	/* Wait to receive a Byte */
	while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_RXNE) == RESET);

	/* Return the Byte read from the SPI bus */
	return (uint8_t) SPI_I2S_ReceiveData(SPI3);
}
