/* Copyright (c) 2013 Nordic Semiconductor. All Rights Reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the license.txt file.
 */

/** @file
 * @brief RTC/PPI/ADC modules example
 *
 * This example demonstrates how to sample voltage with the ADC on an input pin.
 * RTC timer is enabled with a TICK frequency of 10Hz. Each RTC TICK triggers
 * ADC conversion. PPI channel connects the RTC TICK event to the ADC START task.
 * Whenever the ADC samples the voltage on the input pin, the result is displayed
 * on port 1, i.e. LEDs 0-7 (Pins P0.08-P0.15)
 */

#include <stdbool.h>
#include "nrf.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"

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

static void hfclk_config(void)
{
	/* Start 16 MHz crystal oscillator */
  NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
  NRF_CLOCK->TASKS_HFCLKSTART = 1;

  /* Wait for the external oscillator to start up */
  while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0) {}
}

static void lfclk_config(void)
{
  NRF_CLOCK->LFCLKSRC = (CLOCK_LFCLKSRC_SRC_Xtal << CLOCK_LFCLKSRC_SRC_Pos);
  NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;
  NRF_CLOCK->TASKS_LFCLKSTART = 1;
	
	// Wait for the low frequency clock to start
  while (NRF_CLOCK->EVENTS_LFCLKSTARTED == 0) {}
  NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;
}

/** Configure and start RTC
 */
static void rtc_config(void)
{
  NRF_RTC0->PRESCALER = COUNTER_PRESCALER;                   // Set prescaler to a TICK of RTC_FREQUENCY
  NRF_RTC0->EVTENSET = RTC_EVTENSET_TICK_Msk;                // Enable TICK event	
	NRF_RTC0->TASKS_START = 1;                                 // Start RTC0
}

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
	
	// config ADC
	NRF_ADC->CONFIG	= (ADC_CONFIG_EXTREFSEL_None << ADC_CONFIG_EXTREFSEL_Pos) 								/* Bits 17..16 : ADC external reference pin selection. */
										| (ADC_CONFIG_PSEL_AnalogInput6 << ADC_CONFIG_PSEL_Pos)									/*!< Use analog input 6 as analog input (P0.05). */
										| (ADC_CONFIG_REFSEL_VBG << ADC_CONFIG_REFSEL_Pos)											/*!< Use internal 1.2V bandgap voltage as reference for conversion. */
										| (ADC_CONFIG_INPSEL_AnalogInputNoPrescaling << ADC_CONFIG_INPSEL_Pos) 	/*!< Analog input specified by PSEL with no prescaling used as input for the conversion. */
										| (ADC_CONFIG_RES_8bit << ADC_CONFIG_RES_Pos);													/*!< 8bit ADC resolution. */ 	
	
	
	/* Enable ADC*/
	NRF_ADC->ENABLE = ADC_ENABLE_ENABLE_Enabled;	
}

/* Interrupt handler for ADC data ready event. It will be executed when ADC sampling is complete */
void ADC_IRQHandler(void)
{
	/* Clear dataready event */
  NRF_ADC->EVENTS_END = 0;	

  /* Write ADC result to port 1 */
 	nrf_gpio_port_write(NRF_GPIO_PORT_SELECT_PORT1, NRF_ADC->RESULT); 
	
	//Stop the external crystal
	NRF_CLOCK->TASKS_HFCLKSTOP = 1;
}


/**
 * main function
 */
int main(void)
{
	hfclk_config();     //Enable 16MHz crystal for maximum ADC accuracy. Comment out this line to use internal 16MHz RC instead, which saves power.
  gpio_config();			//Configure LEDs
	lfclk_config();			//Configure 32kHz clock, required by the RTC timer
  rtc_config();				//Configure RTC timer
	ppi_init();					//Configure PPI channel, which connects RTC TICK event and ADC START task
	ADC_init();					//Configure the ADC
	
  while (true)
  {	
		//Put CPU to sleep while waiting for interrupt
		__WFI();
  }
}

