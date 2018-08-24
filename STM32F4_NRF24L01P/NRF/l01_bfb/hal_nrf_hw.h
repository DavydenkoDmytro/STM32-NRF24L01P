/*
 * hal_nrf_hw.h
 *
 *  Created on: 11 июл. 2018 г.
 *      Author: Dima
 */

#ifndef L01_BFB_HAL_NRF_HW_H_
#define L01_BFB_HAL_NRF_HW_H_

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"

// Пины, к которым подключены служебные сигналы CSN и CE
#define NRF24_CSN_Port          GPIOE
#define NRF24_CSN_Pin           GPIO_Pin_2
#define NRF24_CE_Port           GPIOE
#define NRF24_CE_Pin            GPIO_Pin_3

/** Macro that set radio's CSN line HIGH.
 *
 */
#define NRF24_CSN_HIGH()        GPIO_WriteBit(NRF24_CSN_Port, NRF24_CSN_Pin, Bit_SET)

/** Macro that set radio's CSN line LOW.
 *
 */
#define NRF24_CSN_LOW()         GPIO_WriteBit(NRF24_CSN_Port, NRF24_CSN_Pin, Bit_RESET)

/** Macro that set radio's CE line HIGH.
 *
 */
#define NRF24_CE_HIGH()         GPIO_WriteBit(NRF24_CE_Port, NRF24_CE_Pin, Bit_SET)

/** Macro that set radio's CE line LOW.
 *
 */
#define NRF24_CE_LOW()          GPIO_WriteBit(NRF24_CE_Port, NRF24_CE_Pin, Bit_RESET)

/**
 * Pulses the CE to nRF24L01 for at least 10 us


#define CE_PULSE()  NRF24_CE_HIGH();\
					delay_us(10);  \
    				NRF24_CE_LOW();
*/
  //count = 10 about 10us
#define CE_PULSE() do { \
	uint8_t tick = 0;\
    uint8_t count; \
    count = 10; \
    NRF24_CE_HIGH();  \
    while (count--){\
        while (tick < 6){\
          tick++;}\
        tick = 0;\
      } \
      NRF24_CE_LOW();  \
    } while(0)

// Инициализация ножек CE и CSN
void nrf24_conrtol_pin_init(void);//, uint8_t Channel);

#endif /* L01_BFB_HAL_NRF_HW_H_ */
