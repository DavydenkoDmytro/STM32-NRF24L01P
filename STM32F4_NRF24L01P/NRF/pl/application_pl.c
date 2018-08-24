/* Copyright (c) 2007 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT. 
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRENTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 * $LastChangedRevision: 2311 $
 */ 

/** @ingroup PL 
 * @file
 * Application to show the functionality of the Enhanced ShockBurst mode with 
 * Bidirectional data. 
 *
 * @b PTX @b mode: 
 * - Sends a packet aprox every 100ms
 * - Accepts user interaction at Button 1
 *   - Not pressed: Send @c 00
 *   - Pressed: Send @c 10
 * - If a ACK was recieved, blink LED2
 * - If a ACK was not recieved, blink LED3
 * - If a ACK payload is recieved, turn on lights according to packet content:
 *    - 0: LED1 off
 *    - 1: LED1 on
 *
 * @b PRX @b mode:
 * - Constantly checks for data
 * - If no packet recieved in aprox 100ms, turn off all leds
 * - If a packet is recieved, turn on lights according to packet content:
 *    - 0: LED1 off
 *    - 1: LED1 on
 * - Accepts user interaction at Button 1
 *   - Not pressed: Put @c 00 in ACK payload
 *   - Pressed: Put @c 10 in ACK payload
 * - If a ACK payload was sent, blink LED2
 * - If a ACK payload was not sent, blink LED3
 *
 * @author Per Kristian Schanke
 */

#include "hal_nrf.h"
#include "radio.h"
#include "application_pl.h"
#include "delay.h"
#include "uart.h"
#include "led.h"
#include "systick_timer.h"

#include <stdlib.h>

/** The data to send in ShockBurst with Bidirectional data mode */
static uint8_t pload_pl[RF_PAYLOAD_LENGTH];

void device_ptx_mode_pl(void)
{
	while (true)
	{
		// Wait til the packet is sent
		do
		{
			radio_irq();
		}
		while ((radio_get_status()) == RF_BUSY);

		// Blink LED2 if ACK is recieved, LED3 if not
		if (((radio_get_status()) == RF_TX_DS)
				|| ((radio_get_status()) == RF_TX_AP))
		{
			uart_send_string(COM1, "LED2_BLINK();", CRLF);
		}
		else
		{
			uart_send_string(COM1, "LED3_BLINK();", CRLF);
		}

		// If ACK payload was recieved, get the payload
		if (radio_get_status() == RF_TX_AP)
		{
			// Get the payload from the PRX and set LED1 accordingly
			if (radio_get_pload_byte(0) == 1)
			{
				LED_ON();
				uart_send_string(COM1, "LED1_ON();", CRLF);
			}
			else
			{
				LED_OFF();
				uart_send_string(COM1, "LED1_OFF();", CRLF);
			}
		}

		// Sleep 100ms
		delay_ms(100);

		// Set up the payload according to the input button 1
		pload_pl[0] = 0;

		if (rand() % 2)//simulate button
		{
			pload_pl[0] = 1;
		}

		//Send the packet
		radio_send_packet(pload_pl, RF_PAYLOAD_LENGTH);
	}
}

void device_prx_mode_pl(void)
{
	NRF24_CE_HIGH();       // Set Chip Enable (CE) pin high to enable reciever

	while (true)
	{
		// Setup and put the ACK payload on the FIFO
		pload_pl[0] = 0;

		if (rand() % 2)       //Key_Scan(GPIOA,GPIO_Pin_0)==KEY_ON)
		{
			pload_pl[0] = 1;
		}

		hal_nrf_write_ack_pload(0, pload_pl, RF_PAYLOAD_LENGTH);

		mdelay_timer(130);

		// Run until either 130ms has lapsed
		// OR there is data on the radio
		do
		{
			radio_irq();
		}
		while ((radio_get_status() == RF_IDLE) && get_TimingDelay());

		// Blink LED2 if ACK payload was sent, LED3 if not
		if ((radio_get_status()) == RF_TX_DS
				|| (radio_get_status()) == RF_TX_AP)
		{
			uart_send_string(COM1, "LED2_BLINK();", CRLF);
		}
		else
		{
			uart_send_string(COM1, "LED3_BLINK();", CRLF);
		}

		if ((radio_get_status()) == RF_RX_DR
				|| (radio_get_status()) == RF_TX_AP)
		{
			// Get the payload from the PTX and set LED1 accordingly
			if (radio_get_pload_byte(0) == 1)
			{
				uart_send_string(COM1, "LED1_ON();", CRLF);
				LED_ON();
			}
			else
			{
				uart_send_string(COM1, "LED1_OFF();", CRLF);
				LED_OFF();
			}
		}
		else
		{
			uart_send_string(COM1, "LED1_OFF();", CRLF);
			LED_OFF();
		}

		// Set radio status to idle
		radio_set_status(RF_IDLE);
	}
}
