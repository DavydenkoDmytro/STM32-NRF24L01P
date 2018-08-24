/*
 * hal_nrf_hw.c
 *
 *  Created on: 11 июл. 2018 г.
 *      Author: Dima
 */

#include "hal_nrf_hw.h"
#include "gpio.h"

void nrf24_conrtol_pin_init(void)
{
	// Инициализируем ножки CE, CSN
	gpio_PortClockStart(NRF24_CSN_Port);
	gpio_PortClockStart(NRF24_CE_Port);
	gpio_SetGPIOmode_Out(NRF24_CSN_Port, NRF24_CSN_Pin);
	gpio_SetGPIOmode_Out(NRF24_CE_Port, NRF24_CE_Pin);

	NRF24_CSN_HIGH();
	NRF24_CE_LOW();

}
