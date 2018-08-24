/*
 * systick_timer.h
 *
 *  Created on: 2 èþë. 2018 ã.
 *      Author: Dima
 */

#ifndef SYSTICK_TIMER_SYSTICK_TIMER_H_
#define SYSTICK_TIMER_SYSTICK_TIMER_H_


void mdelay_timer(unsigned long nTime);
uint8_t get_TimingDelay(void);
void SysTick_Ini(void);

#endif /* SYSTICK_TIMER_SYSTICK_TIMER_H_ */
