/*
 *	Файл:	uart.c
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

#include "uart.h"

UART_RX_t UART_RX[UART_ANZ];
//-------------------------------------------------------------------------------------------------------------------------------
// Определение всех UARTs, как в UART_NAME_t
UART_t UART[] =
{
//	 Имя,  Тактирование         , AF-UART       , UART  , бод   , Прерывание
	{COM1, RCC_APB2Periph_USART1, GPIO_AF_USART1, USART1, 115200, USART1_IRQn, // UART1 mit 115200 Baud

//	 Порт , Контакт    , Тактирование        , Источник
	{GPIOA, GPIO_Pin_9 , RCC_AHB1Periph_GPIOA, GPIO_PinSource9 },  // TX an PA9
	{GPIOA, GPIO_Pin_10, RCC_AHB1Periph_GPIOA, GPIO_PinSource10}}, // RX an PA10
};

//-------------------------------------------------------------------------------------------------------------------------------
// Инициализация всех UARTs
void uart_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	UART_NAME_t nr;
	
	for(nr=0; nr<UART_ANZ; nr++)
	{
		// Clock enable der TX und RX Pins
		RCC_AHB1PeriphClockCmd(UART[nr].TX.CLK, ENABLE);
		RCC_AHB1PeriphClockCmd(UART[nr].RX.CLK, ENABLE);
		// Clock enable der UART
		if((UART[nr].UART==USART1) || (UART[nr].UART==USART6))
		{
			RCC_APB2PeriphClockCmd(UART[nr].CLK, ENABLE);
		}
		else
		{
			RCC_APB1PeriphClockCmd(UART[nr].CLK, ENABLE);
		}
		// UART Alternative-Funktions mit den IO-Pins verbinden
		GPIO_PinAFConfig(UART[nr].TX.PORT,UART[nr].TX.SOURCE,UART[nr].AF);
		GPIO_PinAFConfig(UART[nr].RX.PORT,UART[nr].RX.SOURCE,UART[nr].AF);
		// UART als Alternative-Funktion mit PushPull
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		// TX-Pin
		GPIO_InitStructure.GPIO_Pin = UART[nr].TX.PIN;
		GPIO_Init(UART[nr].TX.PORT, &GPIO_InitStructure);
		// RX-Pin
		GPIO_InitStructure.GPIO_Pin =  UART[nr].RX.PIN;
		GPIO_Init(UART[nr].RX.PORT, &GPIO_InitStructure);
		// Oversampling
		USART_OverSampling8Cmd(UART[nr].UART, ENABLE);
		// init mit Baudrate, 8Databits, 1Stopbit, keine Paritдt, kein RTS+CTS
		USART_InitStructure.USART_BaudRate = UART[nr].BAUD;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;
		USART_InitStructure.USART_StopBits = USART_StopBits_1;
		USART_InitStructure.USART_Parity = USART_Parity_No;
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
		USART_Init(UART[nr].UART, &USART_InitStructure);
		// UART enable
		USART_Cmd(UART[nr].UART, ENABLE);
		// RX-Interrupt enable
		USART_ITConfig(UART[nr].UART, USART_IT_RXNE, ENABLE);
		// enable UART Interrupt-Vector
		NVIC_InitStructure.NVIC_IRQChannel = UART[nr].INT;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
		// RX-Puffer vorbereiten
		UART_RX[nr].rx_buffer[0]=RX_END_CHR;
		UART_RX[nr].wr_ptr=0;
		UART_RX[nr].status=RX_UART_EMPTY;
	}
}

//-------------------------------------------------------------------------------------------------------------------------------
// Послать один байт через UART
void uart_send_byte(UART_NAME_t uart, uint16_t wert)
{
	// Должен был ждать, пока старый байт не отправлено
	while (USART_GetFlagStatus(UART[uart].UART, USART_FLAG_TXE) == RESET);
	USART_SendData(UART[uart].UART, wert);
}

//-------------------------------------------------------------------------------------------------------------------------------
// Отправить строку с помощью UART
void uart_send_string(UART_NAME_t uart, char *ptr, UART_LASTBYTE_t end_cmd)
{
	// Отправить полную строку
	while (*ptr != 0)
	{
		uart_send_byte(uart, *ptr);
		ptr++;
	}
	// Отправим конечный идентификатор
	if(end_cmd == LFCR)
	{
		uart_send_byte(uart,0x0A); // Отправить Line Feed
		uart_send_byte(uart,0x0D); // Отправить возврат каретки
	}
	else if(end_cmd == CRLF)
	{
		uart_send_byte(uart,0x0D); // Отправить возврат каретки
		uart_send_byte(uart,0x0A); // Отправить Line Feed
	}
	else if(end_cmd == LF)
	{
		uart_send_byte(uart,0x0A); // Отправить Line Feed
	}
	else if(end_cmd == CR)
	{
		uart_send_byte(uart,0x0D); // Отправить возврат каретки
	}
}

//-------------------------------------------------------------------------------------------------------------------------------
// Отправить число с помощью UART
void uart_send_int(UART_NAME_t uart, signed long data, UART_LASTBYTE_t end_cmd)
{
	
	unsigned char temp[10], count=0;

	if (data<0)
	{
		data = -data;
		uart_send_byte(uart,'-');
	}
	if (data)
	{
		while (data)
		{
			temp[count++] = data %10 +'0';
			data /= 10;
		}
		while (count)
			uart_send_byte(uart, temp[--count]);
	}
	else uart_send_byte(uart,'0');
	// Отправим конечный идентификатор
	if(end_cmd == LFCR)
	{
		uart_send_byte(uart,0x0A); // Отправить Line Feed
		uart_send_byte(uart,0x0D); // Отправить возврат каретки
	}
	else if(end_cmd == CRLF)
	{
		uart_send_byte(uart,0x0D); // Отправить возврат каретки
		uart_send_byte(uart,0x0A); // Отправить Line Feed
	}
	else if(end_cmd == LF)
	{
		uart_send_byte(uart,0x0A); // Отправить Line Feed
	}
	else if(end_cmd == CR)
	{
		uart_send_byte(uart,0x0D); // Отправить возврат каретки
	}
}

//-------------------------------------------------------------------------------------------------------------------------------
// Отправить HEX число с помощью UART
void uart_send_hex8(UART_NAME_t uart, uint16_t num, UART_LASTBYTE_t end_cmd)
{
	uart_send_byte(uart, HEX_CHARS[(num >> 4)   % 0x10]);
	uart_send_byte(uart, HEX_CHARS[(num & 0x0f) % 0x10]);
	// Отправим конечный идентификатор
	if(end_cmd == LFCR)
	{
		uart_send_byte(uart,0x0A); // Отправить Line Feed
		uart_send_byte(uart,0x0D); // Отправить возврат каретки
	}
	else if(end_cmd == CRLF)
	{
		uart_send_byte(uart,0x0D); // Отправить возврат каретки
		uart_send_byte(uart,0x0A); // Отправить Line Feed
	}
	else if(end_cmd == LF)
	{
		uart_send_byte(uart,0x0A); // Отправить Line Feed
	}
	else if(end_cmd == CR)
	{
		uart_send_byte(uart,0x0D); // Отправить возврат каретки
	}
}

//-------------------------------------------------------------------------------------------------------------------------------
// Отправить HEX число с помощью UART
void uart_send_hex16(UART_NAME_t uart, uint16_t num, UART_LASTBYTE_t end_cmd)
{
	uint8_t i;
	for (i = 12; i > 0; i -= 4) uart_send_byte(uart, (HEX_CHARS[(num >> i) % 0x10]));
	uart_send_byte(uart, (HEX_CHARS[(num & 0x0f) % 0x10]));
	// Отправим конечный идентификатор
	if(end_cmd == LFCR)
	{
		uart_send_byte(uart,0x0A); // Отправить Line Feed
		uart_send_byte(uart,0x0D); // Отправить возврат каретки
	}
	else if(end_cmd == CRLF)
	{
		uart_send_byte(uart,0x0D); // Отправить возврат каретки
		uart_send_byte(uart,0x0A); // Отправить Line Feed
	}
	else if(end_cmd == LF)
	{
		uart_send_byte(uart,0x0A); // Отправить Line Feed
	}
	else if(end_cmd == CR)
	{
		uart_send_byte(uart,0x0D); // Отправить возврат каретки
	}
}

//-------------------------------------------------------------------------------------------------------------------------------
// Отправить HEX число с помощью UART
void uart_send_hex32(UART_NAME_t uart, uint32_t num, UART_LASTBYTE_t end_cmd)
{
	uint8_t i;
	for (i = 28; i > 0; i -= 4)	uart_send_byte(uart, (HEX_CHARS[(num >> i) % 0x10]));
	uart_send_byte(uart, (HEX_CHARS[(num & 0x0f) % 0x10]));
	// Отправим конечный идентификатор
	if(end_cmd == LFCR)
	{
		uart_send_byte(uart,0x0A); // Отправить Line Feed
		uart_send_byte(uart,0x0D); // Отправить возврат каретки
	}
	else if(end_cmd == CRLF)
	{
		uart_send_byte(uart,0x0D); // Отправить возврат каретки
		uart_send_byte(uart,0x0A); // Отправить Line Feed
	}
	else if(end_cmd == LF)
	{
		uart_send_byte(uart,0x0A); // Отправить Line Feed
	}
	else if(end_cmd == CR)
	{
		uart_send_byte(uart,0x0D); // Отправить возврат каретки
	}
}

//-------------------------------------------------------------------------------------------------------------------------------
// Получение строки с помощью UART(обрабатывается прерыванием). Эта функция должна быть очищена циклически
// Возвращаемое значение:
//  -> если строка не получена	= RX_EMPTY
//  -> если строка получена 	= RX_READY -> Строка в *ptr
//  -> сли буфер полон      	= RX_FULL
UART_RXSTATUS_t uart_receive_string(UART_NAME_t uart, char *ptr)
{
	UART_RXSTATUS_t ret_wert=RX_UART_EMPTY;
	uint8_t n,wert;

	if(UART_RX[uart].status==RX_UART_READY)
	{
		ret_wert=RX_UART_READY;
		// буфер копирования
		n=0;
		do
		{
			wert=UART_RX[uart].rx_buffer[n];
			if(wert!=RX_END_CHR)
			{
				ptr[n]=wert;
				n++;
			}
		}
		while(wert!=RX_END_CHR);
		// Конец строки идентификатора
		ptr[n]=0x00;
		// RX-буфер освобождение
		UART_RX[uart].rx_buffer[0]=RX_END_CHR;
		UART_RX[uart].wr_ptr=0;
		UART_RX[uart].status=RX_UART_EMPTY;
	}
	else if(UART_RX[uart].status==RX_UART_FULL)
	{
		ret_wert=RX_UART_FULL;
		// RX-буфер освобождение
		UART_RX[uart].rx_buffer[0]=RX_END_CHR;
		UART_RX[uart].wr_ptr=0;
		UART_RX[uart].status=RX_UART_EMPTY;
	}
	return(ret_wert);
}

//-------------------------------------------------------------------------------------------------------------------------------
// Внутренняя функция. Хранить принятый символ в буфере
void P_UART_Receive(UART_NAME_t uart, uint16_t wert)
{
	if(UART_RX[uart].wr_ptr<RX_BUF_SIZE)
	{
		// Если еще место в буфере
		if(UART_RX[uart].status==RX_UART_EMPTY)
		{
			// Если нет конца идентификатора получено не было
			if((wert>=RX_FIRST_CHR) && (wert<=RX_LAST_CHR))
			{
				// Сохранить байт в буфере
				UART_RX[uart].rx_buffer[UART_RX[uart].wr_ptr]=wert;
				UART_RX[uart].wr_ptr++;
			}
			if(wert==RX_END_CHR)
			{
				// Если конечный идентификатор получен
				UART_RX[uart].rx_buffer[UART_RX[uart].wr_ptr]=wert;
				UART_RX[uart].status=RX_UART_READY;
			}
		}
	}
	else
	{
		// Когда буфер полон
		UART_RX[uart].status=RX_UART_FULL;
	}
}

//-------------------------------------------------------------------------------------------------------------------------------
// Внутренняя функция. Функция прерывания UART
// Номер прерывания должен быть передан
void P_UART_RX_INT(uint8_t int_nr, uint16_t wert)
{
	UART_NAME_t nr;

	// Перебираем UART
	for(nr=0; nr<UART_ANZ; nr++)
	{
		if(UART[nr].INT==int_nr)
		{
			// Найденна запись, сохранить байт
			P_UART_Receive(nr,wert);
			break;
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------------------
// UART1-Прерывание
/*void USART1_IRQHandler(void)
{
	uint16_t wert;

	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
	{
		// Если байт в приемном буфере
		wert=USART_ReceiveData(USART1);
		// Сохранить байт
		P_UART_RX_INT(USART1_IRQn,wert);
	}
}*/

//-------------------------------------------------------------------------------------------------------------------------------
// UART2-Прерывание
void USART2_IRQHandler(void)
{
	uint16_t wert;

	if (USART_GetITStatus(USART2, USART_IT_RXNE) == SET)
	{
		// Если байт в приемном буфере
		wert=USART_ReceiveData(USART2);
		// Сохранить байт
		P_UART_RX_INT(USART2_IRQn,wert);
	}
}

//-------------------------------------------------------------------------------------------------------------------------------
// UART3-Interrupt
void USART3_IRQHandler(void)
{
	uint16_t wert;

	if (USART_GetITStatus(USART3, USART_IT_RXNE) == SET)
	{
		// Если байт в приемном буфере
		wert=USART_ReceiveData(USART3);
		// Сохранить байт
		P_UART_RX_INT(USART3_IRQn,wert);
	}
}

//-------------------------------------------------------------------------------------------------------------------------------
// UART4-Interrupt
void UART4_IRQHandler(void)
{
	uint16_t wert;

	if (USART_GetITStatus(UART4, USART_IT_RXNE) == SET)
	{
		// Если байт в приемном буфере
		wert=USART_ReceiveData(UART4);
		// Сохранить байт
		P_UART_RX_INT(UART4_IRQn,wert);
	}
}

//-------------------------------------------------------------------------------------------------------------------------------
// UART5-Interrupt
void UART5_IRQHandler(void)
{
	uint16_t wert;

	if (USART_GetITStatus(UART5, USART_IT_RXNE) == SET)
	{
		// Если байт в приемном буфере
		wert=USART_ReceiveData(UART5);
		// Сохранить байт
		P_UART_RX_INT(UART5_IRQn,wert);
	}
}

//-------------------------------------------------------------------------------------------------------------------------------
// UART6-Interrupt
void USART6_IRQHandler(void)
{
	uint16_t wert;

	if (USART_GetITStatus(USART6, USART_IT_RXNE) == SET)
	{
		// Если байт в приемном буфере
		wert=USART_ReceiveData(USART6);
		// Сохранить байт
		P_UART_RX_INT(USART6_IRQn,wert);
	}
}
