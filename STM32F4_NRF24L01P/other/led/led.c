/*
 * led.c
 *
 *  Created on: 2 èþë. 2018 ã.
 *      Author: Dima
 */

#include "gpio.h"
#include "led.h"

void Led_Ini(void)
{
	gpio_PortClockStart(LED_Port);
	gpio_SetGPIOmode_Out(LED_Port, LED_Pin);
	LED_OFF();
}
