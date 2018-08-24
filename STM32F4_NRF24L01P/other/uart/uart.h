/*
 *	Файл:	uart.h
 *	Дата:	23.03.2016
 */
//
//	Возможные разводки UART
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

// Список UARTs (начать с 0 не дублируется)
typedef enum
{
	COM1 = 0	// COM1 (TX=PA9, RX=PA10)
}UART_NAME_t;

#define  UART_ANZ   1 // Anzahl von UART_NAME_t

// Конечный идентификатор при отправке
typedef enum
{
	NONE = 0,	// Нет идентификатора
	LFCR,		// Линия подачи + возврат каретки (0x0A,0x0D)
	CRLF,		// Возврат каретки + линия подачи (0x0D,0x0A)
	LF,			// Линия подачи (0x0A)
	CR			// Возврат каретки (0x0D)
}UART_LASTBYTE_t;

// Состояние при получении
typedef enum
{
	RX_UART_EMPTY = 0,	// буфер пуст
	RX_UART_READY,		// что-то в приемном буфере
	RX_UART_FULL			// RX буфер полон
}UART_RXSTATUS_t;

// Структура UART-Подключения
typedef struct
{
	GPIO_TypeDef* PORT;		// Порт
	const uint16_t PIN;     // Контакт
	const uint32_t CLK;     // Тактирование
	const uint8_t SOURCE;   // Источник
}UART_PIN_t;

// Структура UARTs
typedef struct
{
	UART_NAME_t UART_NAME;	// Имя
	const uint32_t CLK;		// Тактирование
	const uint8_t AF;		// Алтернативная функция
	USART_TypeDef* UART;	// UART
	const uint32_t BAUD;	// Битрэйд
	const uint8_t INT;		// Прерывания
	UART_PIN_t TX;			// TX-Контакт
	UART_PIN_t RX;			// RX-Контакт
}UART_t;

// Определяется для приема
#define  RX_BUF_SIZE   50    // Размер буфера RX в байтах
#define  RX_FIRST_CHR  0x20  // Первые разрешенные символы (значение ASCII)
#define  RX_LAST_CHR   0x7E  // последние разрешенные символы (значение ASCII)
#define  RX_END_CHR    0x0D  // Конечный идентификатор (значение ASCII)
#define	 HEX_CHARS     "0123456789ABCDEF"	// Символы для HEX
// Структура для UART RX
typedef struct
{
	char rx_buffer[RX_BUF_SIZE]; // RX буфера
	uint8_t wr_ptr;              // написать указатель
	UART_RXSTATUS_t status;      // статус RX
}UART_RX_t;


// Глобальные функции
void uart_init(void);																// Инициализация всех UARTs
void uart_send_byte(UART_NAME_t uart, uint16_t wert);								// Послать один байт через UART
void uart_send_string(UART_NAME_t uart, char *ptr, UART_LASTBYTE_t end_cmd);		// Отправить строку с помощью UART
void uart_send_int(UART_NAME_t uart, signed long data, UART_LASTBYTE_t end_cmd);	// Отправить число с помощью UART
void uart_send_hex8(UART_NAME_t uart, uint16_t num, UART_LASTBYTE_t end_cmd);		// Отправить HEX число с помощью UART
void uart_send_hex16(UART_NAME_t uart, uint16_t num, UART_LASTBYTE_t end_cmd);		// Отправить HEX число с помощью UART
void uart_send_hex32(UART_NAME_t uart, uint32_t num, UART_LASTBYTE_t end_cmd);		// Отправить HEX число с помощью UART
UART_RXSTATUS_t uart_receive_string(UART_NAME_t uart, char *ptr);					// Получение строки с помощью UART

#endif // __UART_H
