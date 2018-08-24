/*
 * systick_timer.c
 *
 *  Created on: 2 èþë. 2018 ã.
 *      Author: Dima
 */

#include "stm32f4xx.h"
#include "systick_timer.h"

#define TICK_FREQ (1000u)

volatile uint32_t g_ul_ms_ticks=0;
static volatile uint32_t TimingDelay=0;
unsigned long idle_time=0;
extern uint32_t SystemCoreClock; //168000000=168Mhz (original value)

void mdelay_timer(unsigned long nTime)
{
	TimingDelay = nTime;
}

uint8_t get_TimingDelay(void)
{
	return TimingDelay;
}

void TimingDelay_Decrement(void)
{
	if (TimingDelay != 0x00)
		TimingDelay--;
}

void SysTick_Handler(void)
{
  	TimingDelay_Decrement();
}

void SysTick_Ini(void)
{
	if (SysTick_Config(SystemCoreClock / TICK_FREQ)) // Setup SysTick Timer for 1 msec interrupts
	{
		while (1) ;// Handle Error
	}
}

