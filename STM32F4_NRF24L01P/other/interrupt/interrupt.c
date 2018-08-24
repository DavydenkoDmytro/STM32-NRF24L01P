/*
 * interrupt.c
 *
 *  Created on: 2 èþë. 2018 ã.
 *      Author: Dima
 */

#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_exti.h"

#include "interrupt.h"

volatile static uint8_t interrupt_state = 0;

#define INVEN_INT_PIN                         GPIO_Pin_9
#define INVEN_INT_GPIO_PORT                   GPIOB
#define INVEN_INT_GPIO_CLK                    RCC_AHB1Periph_GPIOB
#define INVEN_INT_EXTI_PORT                   EXTI_PortSourceGPIOB
#define INVEN_INT_EXTI_PIN                    EXTI_PinSource9
#define INVEN_INT_EXTI_LINE                   EXTI_Line9
#define INVEN_INT_EXTI_IRQ                    EXTI9_5_IRQn

void Setup_Interrupt_NRF_IRQ_PIN(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;

	/* Enable GPIOB clock */
	RCC_AHB1PeriphClockCmd(INVEN_INT_GPIO_CLK, ENABLE);
	/* Enable SYSCFG clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	/* Configure invensense sensor interrupt pin as input floating */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Pin = INVEN_INT_PIN;
	GPIO_Init(INVEN_INT_GPIO_PORT, &GPIO_InitStructure); //GPIOE

	/* Connect EXTI Line to inv sensor interrupt pin */
	SYSCFG_EXTILineConfig(INVEN_INT_EXTI_PORT, INVEN_INT_EXTI_PIN);

	/* Configure EXTI Line1 */
	EXTI_InitStructure.EXTI_Line = INVEN_INT_EXTI_LINE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* Enable and set EXTI Line Interrupt to the highest priority */
	NVIC_InitStructure.NVIC_IRQChannel = INVEN_INT_EXTI_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

uint8_t state_of_interrupt(void)
{
	return interrupt_state;
}

void reset_state_of_interrupt(void)
{
	interrupt_state = 0;
}

void EXTI9_5_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line9) != RESET)
	{
		interrupt_state = 1;
		EXTI_ClearITPendingBit(EXTI_Line9);
	}
}
