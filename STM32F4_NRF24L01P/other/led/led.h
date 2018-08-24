/*
 * led.h
 *
 *  Created on: 2 èþë. 2018 ã.
 *      Author: Dima
 */

#ifndef LED_LED_H_
#define LED_LED_H_

#define LED_Port        GPIOE
#define LED_Pin         GPIO_Pin_4
#define LED_ON()        GPIO_WriteBit(LED_Port, LED_Pin, Bit_SET)
#define LED_OFF()       GPIO_WriteBit(LED_Port, LED_Pin, Bit_RESET)

void Led_Ini(void);

#endif /* LED_LED_H_ */
