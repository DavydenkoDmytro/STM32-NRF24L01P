//------------------------------------------------------------------------------
// This is Open source software. You can place this code on your site, but don't
// forget a link to my YouTube-channel: https://www.youtube.com/channel/UChButpZaL5kUUl_zTyIDFkQ
// ��� ����������� ����������� ���������������� ��������. �� ������ ���������
// ��� �� ����� �����, �� �� �������� ������� ������ �� ��� YouTube-����� 
// "����������� � ���������" https://www.youtube.com/channel/UChButpZaL5kUUl_zTyIDFkQ
// �����: �������� ������ / Nadyrshin Ruslan
//------------------------------------------------------------------------------
#ifndef _SPIM_H
#define _SPIM_H

//#include "../types.h"
#include <stm32f4xx_spi.h>

// ��������� ������������� spi (SPI1 ��� SPI2) � ������ master
void spim_init(SPI_TypeDef* SPIx, uint8_t WordLen);
// ��������� ���������� ������ 16-������ ����
void SPI_send16b(SPI_TypeDef* SPIx, uint16_t *pBuff, uint16_t Len);
// ��������� ���������� ������ 8-������ ����
void SPI_send8b(SPI_TypeDef* SPIx, uint8_t *pBuff, uint16_t Len);
// ��������� ���������� ������ 16-������ ����
void SPI_recv16b(SPI_TypeDef* SPIx, uint16_t *pBuff, uint16_t Len);
// ��������� ���������� ������ 8-������ ����
void SPI_recv8b(SPI_TypeDef* SPIx, uint8_t *pBuff, uint16_t Len);
// ��������� ���������� 1 ���� � ���������� �������� ����
uint8_t SPI_SendRecvByte(SPI_TypeDef* SPIx, uint8_t TxByte);

// ��������� ����������  8-������ ����
void SPI_send_one_byte(SPI_TypeDef* SPIx, uint8_t data);
// ��������� ������ 1-����
uint8_t SPI_read_one_byte(SPI_TypeDef* SPIx);
// ��������� ���������� 1 ���� � ���������� �������� ����
uint8_t SPI_SendRecvByte(SPI_TypeDef* SPIx, uint8_t TxByte);

uint8_t hal_nrf_rw(uint8_t data);

#endif
