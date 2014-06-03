/* Copyright (c) 2013 Nordic Semiconductor. All Rights Reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the license.txt file.
 */

/** @file
 * @brief RTC/PPI/ADC modules example
 *
 * This example enables the RTC with a TICK frequency of 10Hz. Each RTC TICK triggers
 * ADC conversion through a PPI channel. The supply voltage for the chip is sampled
 * with the ADC. To change the ADC output, adjust the Supply voltage for the nRF6310
 * motherboard in nRFgo Studio.
 */

#include <stdbool.h>
#include "nrf.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"

#define GPIO_TOGGLE_TICK_EVENT    (8)                       /*!< Pin number to toggle when there is a tick event in RTC */
#define LFCLK_FREQUENCY           (32768UL)                 /*!< LFCLK frequency in Hertz, constant */
#define RTC_FREQUENCY             (10UL)                     /*!< required RTC working clock RTC_FREQUENCY Hertz. Changable */
#define COUNTER_PRESCALER         ((LFCLK_FREQUENCY/RTC_FREQUENCY) - 1)  /* f = LFCLK/(prescaler + 1) */

/** Configures port 1 as output
 */
static void gpio_config(void)
{
  nrf_gpio_range_cfg_output(8, 15);
  nrf_gpio_port_write(NRF_GPIO_PORT_SELECT_PORT1, 0);
}

/** Configures and starts the low frequency 32kHz clock (required to operate the RTC)
 */
static void lfclk_config(void)
{
  NRF_CLOCK->LFCLKSRC = (CLOCK_LFCLKSRC_SRC_Xtal << CLOCK_LFCLKSRC_SRC_Pos);
  NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;
  NRF_CLOCK->TASKS_LFCLKSTART = 1;
	
	// Wait for the low frequency clock to start
  while (NRF_CLOCK->EVENTS_LFCLKSTARTED == 0) {}
  NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;
}

/** Configures and starts RTC
 */
static void rtc_config(void)
{
  NRF_RTC0->PRESCALER = COUNTER_PRESCALER;                   // Set prescaler to a TICK of RTC_FREQUENCY
  NRF_RTC0->EVTENSET = RTC_EVTENSET_TICK_Msk;                // Enable TICK event	
	NRF_RTC0->TASKS_START = 1;                                 // Start RTC0
}

/** Configures and enables a PPI channel
 */
static void ppi_init(void)
{
  // Configure PPI channel 0 to start ADC task
  NRF_PPI->CH[0].EEP = (uint32_t)&NRF_RTC0->EVENTS_TICK;
  NRF_PPI->CH[0].TEP = (uint32_t)&NRF_ADC->TASKS_START;

  // Enable PPI channel 0
  NRF_PPI->CHEN = (PPI_CHEN_CH0_Enabled << PPI_CHEN_CH0_Pos);
}

/** Configures and enables the ADC
 */
void ADC_init(void)
{	
	/* Enable interrupt on ADC sample ready event*/		
	NRF_ADC->INTENSET = ADC_INTENSET_END_Msk;
	NVIC_EnableIRQ(ADC_IRQn);	
	
	/* Configure ADC - set reference input source to internal 1.2V bandgap */
	NRF_ADC->CONFIG = (ADC_CONFIG_REFSEL_VBG << ADC_CONFIG_REFSEL_Pos);
	/* Configure ADC - set input source to VDD/3 */
	NRF_ADC->CONFIG |= (ADC_CONFIG_INPSEL_SupplyOneThirdPrescaling << ADC_CONFIG_INPSEL_Pos);
	/* Configure ADC - select 8 bit resolution */
	NRF_ADC->CONFIG |= (ADC_CONFIG_RES_8bit << ADC_CONFIG_RES_Pos);
	
	/* Enable ADC*/
	NRF_ADC->ENABLE = ADC_ENABLE_ENABLE_Enabled;	
}

/* Interrupt handler for ADC data ready event */
void ADC_IRQHandler(void)
{
	/* Clear dataready event */
  NRF_ADC->EVENTS_END = 0;	

  /* Write ADC result to port 1 */
 	nrf_gpio_port_write(NRF_GPIO_PORT_SELECT_PORT1, NRF_ADC->RESULT); 
	
	//Use the STOP task to save current. Workaround for PAN_028 rev1.2 anomaly 1.
  NRF_ADC->TASKS_STOP = 1;
}


/**
 * main function
 */
int main(void)
{
  gpio_config();
	lfclk_config();
  rtc_config();
	ppi_init();
	ADC_init();
	
  while (true)
  {	
		//Put CPU to sleep while waiting for interrupt
		__WFI();
  }
}

/**
 *@}
 **/
