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
 * $LastChangedRevision: 2477 $
 */ 

/**
 * @file
 * @ingroup Main
 * This file contain the main initialisation and allows users to make their 
 * choices of operational mode. Implements a state machine through the enum 
 * @c state, the @c state_machine array, and the get_next_state() function.
 *
 * To choose between the differnt modes, after startup press:
 * - B1 for PTX mode
 *   - B1 for ShockBurst in PTX mode. Indicate with @b LED1 on
 *   - B2 for Enhanced ShockBurst in PTX mode. Indicate with @b LED2 on
 *   - B3 for Enhanced ShockBurst with Bidirectional data in PTX mode. Indicate with @b LED1 and @b LED2 on
 * - B2 for PRX mode
 *   - B1 for ShockBurst in PRX mode. Indicate with @b LED1 and @b LED3 on
 *   - B2 for Enhanced ShockBurst in PRX mode. Indicate with @b LED2 and @b LED3 on
 *   - B3 for Enhanced ShockBurst with Bidirectional data in PRX mode. Indicate with @b LED1, @b LED2 and @b LED3 on
 *
 * @author Per Kristian Schanke 
 */

#include <stdint.h>
#include <stdbool.h>

#include "stm32f4xx.h"
#include "main.h"
#include <delay.h>
#include "uart.h"

#include "gpio.h"
#include "interrupt.h"
#include "led.h"
#include "systick_timer.h"
#include "spim.h"

#include "hal_nrf.h"

/** Contain the common radio functions, implemented in radio.c */
#include "radio.h"                
/** Contain the specific radio functions for a radio in ShockBurst, 
 * implemented in sb/radio_sb.c */
#include "radio_sb.h"
/** Contain the specific radio functions for a radio in Enhanced ShockBurst, 
 * implemented in esb/radio_esb.c */
#include "radio_esb.h"
/** Contain the specific radio functions for a radio in Enhanced ShockBurst 
 * with Bidirectional data, implemented in pl/radio_pl.c */
#include "radio_pl.h"
/** Contain the application functions for a radio in ShockBurst, 
 * implemented in sb/application_sb.c */
#include "application_sb.h"
/** Contain the application functions for a radio in Enhanced ShockBurst, 
 * implemented in esb/application_esb.c */
#include "application_esb.h"
/** Contain the application functions for a radio in Enhanced ShockBurst 
 * with Bidirectional data, implemented in pl/application_pl.c */
#include "application_pl.h"
/** Contain the functions for delays, system functions and some timers, 
 * implemented in system.c */
//#include "system.h"


/**
 * The possible states of the system.
 */
typedef enum {
  DEVICE_IDLE = 0, /**< The device is idle */
  DEVICE_PRX_IDLE, /**< The device will operate in @b PRX mode */
  DEVICE_PTX_IDLE, /**< The device will operate in @b PTX mode */
  DEVICE_PRX_SB,   /**< The device will operate in @b PRX mode with ShockBurst functionailty */
  DEVICE_PRX_ESB,  /**< The device will operate in @b PRX mode with Enhanced ShockBurst functionailty */
  DEVICE_PRX_PL,   /**< The device will operate in @b PRX mode with Enhanced ShockBurst functionailty with Bidirectional data */
  DEVICE_PTX_SB,   /**< The device will operate in @b PTX mode with ShockBurst functionailty */
  DEVICE_PTX_ESB,  /**< The device will operate in @b PTX mode with Enhanced ShockBurst functionailty */
  DEVICE_PTX_PL,   /**< The device will operate in @b PTX mode with Enhanced ShockBurst functionailty with Bidirectional data */
  NO_CHANGE        /**< No state change */
} state_t;


/** The address of the radio. Parameter to the radio init */
const uint8_t address[HAL_NRF_AW_5BYTES] = {0x22, 0x33, 0x44, 0x55, 0x01};


/** Function that initialises everything. Calls @b system_init () which is
 * hardware dependant, and @b device_boot_msg () from @b system.c.
 * It implementes a simple statemachine to handle the input from the user on 
 * the evaluation board. With two clicks, the user can choose between 
 * primary transmitter mode (PTX) and primary reciever mode (PRX), and between
 * the functionality levels ShockBurst (sb), Enchanced ShockBurst, 
 * and Enhanced ShockBurst with Bidirectional data (pl).
 */  
int main(void)
{
  state_t current_state = DEVICE_PRX_PL;

  SystemInit();                 //Hardware dependant system initialisation
  uart_init();

  // Configure SPI
  nrf24_conrtol_pin_init();
  spim_init(SPI3, 8);
  Setup_Interrupt_NRF_IRQ_PIN();

  Led_Ini();
  SysTick_Ini();

  uart_send_string(COM1, "STM32 & NRF24L01+", CRLF);

  //Implemenation of a simple state machine. 
  while (1)
  {
    switch (current_state)
    {
      case DEVICE_IDLE:                       // No operation chosen yet
        break;

      case DEVICE_PRX_IDLE:                   // In PRX mode, but still lack 
        break;                                // functionality

      case DEVICE_PTX_IDLE:                   // In PTX mode, but still lack 
        break;                                // functionality

      case DEVICE_PRX_SB:                     // Start as PRX in ShockBurst 
        radio_sb_init (address, HAL_NRF_PRX);
        device_prx_mode_sb ();
        break;

      case DEVICE_PRX_ESB:                    // Start as PRX in Enhanced 
        radio_esb_init (address, HAL_NRF_PRX);// ShockBurst
        device_prx_mode_esb ();
        break;

      case DEVICE_PRX_PL:                     //Start as PRX in Enhanced 
        radio_pl_init (address, HAL_NRF_PRX); //ShockBurst with ACK payload
        device_prx_mode_pl ();
        break;

      case DEVICE_PTX_SB:                     //Start as PTX in ShockBurst
        radio_sb_init (address, HAL_NRF_PTX);
        device_ptx_mode_sb ();
        break;

      case DEVICE_PTX_ESB:                    //Start as PTX in Enhanced 
        radio_esb_init (address, HAL_NRF_PTX);//ShockBurst
        device_ptx_mode_esb ();
        break;

      case DEVICE_PTX_PL:                     // Start as PTX in Enhanced 
        radio_pl_init (address, HAL_NRF_PTX); // ShockBurst with ACK payload
        device_ptx_mode_pl ();
        break;

      default:                                // If in an illegal state, set to
        current_state = DEVICE_IDLE;       // default state (DEVICE_IDLE)
        break;
    }
  }
}


