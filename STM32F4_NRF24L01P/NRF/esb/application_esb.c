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

/** @ingroup ESB 
 * @file
 * Application to show the functionality of the Enhanced ShockBurst mode. 
 *
 * @b PTX @b mode: 
 * - Sends a packet aprox every 100ms
 * - Accepts user interaction at Button 1:
 *   - Not pressed: Send @c 00
 *   - Pressed: Send @c 10
 * - If a ACK was recieved, blink LED2
 * - If a ACK was not recieved, blink LED3
 *
 * @b PRX @b mode:
 * - Constantly checks for data
 * - If no packet recieved in aprox 100ms, turn off all leds
 * - If a packet is recieved, turn on lights according to packet content:
 *    - 0: LED1 off
 *    - 1: LED1 on
 *
 * @author Per Kristian Schanke
 *
 */

#include "hal_nrf.h"
//#include "system.h"
#include "radio.h"
#include "application_esb.h"

#include "delay.h"
#include "systick_timer.h"
#include <stdlib.h>
#include "led.h"
#include "uart.h"

/** The data to send in ShockBurst mode */
static uint8_t pload_esb[RF_PAYLOAD_LENGTH];

void device_ptx_mode_esb(void)
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
		if (((radio_get_status()) == RF_TX_DS))
		{
			uart_send_string(COM1, "LED2_BLINK();", CRLF);

		}
		else
		{
			uart_send_string(COM1, "LED3_BLINK();", CRLF);
		}

		delay_ms(100);

		// Set up the payload according to the input button 1
		pload_esb[0] = 0;

		if (rand() % 2) //simulate button
		{
			pload_esb[0] = 1;
		}

		//Send the packet
		radio_send_packet(pload_esb, RF_PAYLOAD_LENGTH);
	}
}

void device_prx_mode_esb(void)
{
	NRF24_CE_HIGH();// Set Chip Enable (CE) pin high to enable reciever

	while (true)
	{
		mdelay_timer(130);
		// Run until either 110ms has lapsed
		// OR there is data on the radio
		do
		{
			radio_irq();
		}
		while ((radio_get_status() == RF_IDLE) && get_TimingDelay());

		if ((radio_get_status()) == RF_RX_DR)
		{
			// Get the payload from the PTX and set LED1
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
		else
		{
			LED_OFF();
			uart_send_string(COM1, "LED1_OFF();", CRLF);
		}

		// Set radio status to idle
		radio_set_status(RF_IDLE);
	}
}
