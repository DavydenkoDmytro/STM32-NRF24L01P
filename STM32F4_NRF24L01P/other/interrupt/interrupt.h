/*
 * interrupt.h
 *
 *  Created on: 2 èþë. 2018 ã.
 *      Author: Dima
 */

#ifndef INTERRUPT_INTERRUPT_H_
#define INTERRUPT_INTERRUPT_H_

/*
 * PIN GPIO_Pin_9
 *
 * */

void Setup_Interrupt_NRF_IRQ_PIN(void);

uint8_t state_of_interrupt(void);

void reset_state_of_interrupt(void);

#endif /* INTERRUPT_INTERRUPT_H_ */
