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

/** @ingroup SB 
 * @file
 * Application to show the functionality of the ShockBurst mode. 
 *
 * @b PTX @b mode: 
 * - Sends a packet aprox every 100ms
 * - Accepts user interaction at Button 1:
 *   - Not pressed: Send @c 00
 *   - Pressed: Send @c 10
 *
 * @b PRX @b mode:
 * - Constantly checks for data
 * - If no packet recieved in aprox 100ms, turn off all leds
 * - If a packet is recieved, turn on lights according to packet content:
 *    - 0: LED1 off
 *    - 1: LED1 on
 *
 * @author Per Kristian Schanke
 */

#include "hal_nrf.h"
#include "radio.h"
#include "application_sb.h"
#include "delay.h"
#include "led.h"
#include "systick_timer.h"
#include <stdlib.h>

/** The data to send in ShockBurst mode */
static uint8_t pload_sb[RF_PAYLOAD_LENGTH];

void device_ptx_mode_sb(void)
{
	while (true)
	{
		// Wait til the packet is sent
		do
		{
			radio_irq();
		}
		while ((radio_get_status()) == RF_BUSY);

		delay_ms(100);

		// Set up the payload according to the input button 1
		pload_sb[0] = 0;

		if(rand() % 2)//simulate button
		{
			pload_sb[0] = 1;
		}

		//Send the packet
		radio_send_packet(pload_sb, RF_PAYLOAD_LENGTH);
	}
}

void device_prx_mode_sb(void) {

	NRF24_CE_HIGH();

	while (true)
	{
		mdelay_timer(130);

		// Run until either 130ms has lapsed
		// OR there is data on the radio
		do
		{
			radio_irq();
		}
		while ((radio_get_status() == RF_IDLE) && get_TimingDelay());

		if ((radio_get_status()) == RF_RX_DR)
		{
			// Get the payload from the PTX and set LED1 accordingly
			if (radio_get_pload_byte(0) == 1)
			{
				LED_ON();
			}
			else
			{
				LED_OFF();
			}
		}
		else
		{
			LED_OFF();
		}

		// Set radio status to idle
		radio_set_status(RF_IDLE);
	}
}
