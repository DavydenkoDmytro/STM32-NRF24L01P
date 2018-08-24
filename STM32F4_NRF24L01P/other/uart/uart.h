/*
 *	����:	uart.h
 *	����:	23.03.2016
 */
//
//	��������� �������� UART
//	UART1 : TX:[PA9,PB6] RX:[PA10,PB7]
//	UART2 : TX:[PA2,PD5] RX:[PA3,PD6]
//	UART3 : TX:[PB10,PC10,PD8] RX:[PB11,PC11,PD9]
//	UART4 : TX:[PA0,PC10] RX:[PA1,PC11]
//	UART5 : TX:[PC12] RX:[PD2]
//	UART6 : TX:[PC6,PG14] RX:[PC7,PG9]

#ifndef __UART_H
#define __UART_H

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_usart.h"
#include "misc.h"

// ������ UARTs (������ � 0 �� �����������)
typedef enum
{
	COM1 = 0	// COM1 (TX=PA9, RX=PA10)
}UART_NAME_t;

#define  UART_ANZ   1 // Anzahl von UART_NAME_t

// �������� ������������� ��� ��������
typedef enum
{
	NONE = 0,	// ��� ��������������
	LFCR,		// ����� ������ + ������� ������� (0x0A,0x0D)
	CRLF,		// ������� ������� + ����� ������ (0x0D,0x0A)
	LF,			// ����� ������ (0x0A)
	CR			// ������� ������� (0x0D)
}UART_LASTBYTE_t;

// ��������� ��� ���������
typedef enum
{
	RX_UART_EMPTY = 0,	// ����� ����
	RX_UART_READY,		// ���-�� � �������� ������
	RX_UART_FULL			// RX ����� �����
}UART_RXSTATUS_t;

// ��������� UART-�����������
typedef struct
{
	GPIO_TypeDef* PORT;		// ����
	const uint16_t PIN;     // �������
	const uint32_t CLK;     // ������������
	const uint8_t SOURCE;   // ��������
}UART_PIN_t;

// ��������� UARTs
typedef struct
{
	UART_NAME_t UART_NAME;	// ���
	const uint32_t CLK;		// ������������
	const uint8_t AF;		// ������������� �������
	USART_TypeDef* UART;	// UART
	const uint32_t BAUD;	// �������
	const uint8_t INT;		// ����������
	UART_PIN_t TX;			// TX-�������
	UART_PIN_t RX;			// RX-�������
}UART_t;

// ������������ ��� ������
#define  RX_BUF_SIZE   50    // ������ ������ RX � ������
#define  RX_FIRST_CHR  0x20  // ������ ����������� ������� (�������� ASCII)
#define  RX_LAST_CHR   0x7E  // ��������� ����������� ������� (�������� ASCII)
#define  RX_END_CHR    0x0D  // �������� ������������� (�������� ASCII)
#define	 HEX_CHARS     "0123456789ABCDEF"	// ������� ��� HEX
// ��������� ��� UART RX
typedef struct
{
	char rx_buffer[RX_BUF_SIZE]; // RX ������
	uint8_t wr_ptr;              // �������� ���������
	UART_RXSTATUS_t status;      // ������ RX
}UART_RX_t;


// ���������� �������
void uart_init(void);																// ������������� ���� UARTs
void uart_send_byte(UART_NAME_t uart, uint16_t wert);								// ������� ���� ���� ����� UART
void uart_send_string(UART_NAME_t uart, char *ptr, UART_LASTBYTE_t end_cmd);		// ��������� ������ � ������� UART
void uart_send_int(UART_NAME_t uart, signed long data, UART_LASTBYTE_t end_cmd);	// ��������� ����� � ������� UART
void uart_send_hex8(UART_NAME_t uart, uint16_t num, UART_LASTBYTE_t end_cmd);		// ��������� HEX ����� � ������� UART
void uart_send_hex16(UART_NAME_t uart, uint16_t num, UART_LASTBYTE_t end_cmd);		// ��������� HEX ����� � ������� UART
void uart_send_hex32(UART_NAME_t uart, uint32_t num, UART_LASTBYTE_t end_cmd);		// ��������� HEX ����� � ������� UART
UART_RXSTATUS_t uart_receive_string(UART_NAME_t uart, char *ptr);					// ��������� ������ � ������� UART

#endif // __UART_H
