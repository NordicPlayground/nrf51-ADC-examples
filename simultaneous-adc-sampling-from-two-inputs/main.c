/* Copyright (c) 2013 Nordic Semiconductor. All Rights Reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the license.txt file.
 */

/* ADC example. */
/* Uses the 1.2V fixed reference input. Input voltage is from both ADC input pin nr 2 (P0.01) and ADC input pin 6 (P0.05)*/
/* Input pin (button 1) can be toggled to show ADC conversion result for either pin 2 or pin 6 on output pins 8-15 */
/* The ADC peripheral is powered up before each conversion and then powered down after conversion */
/* To test functionality, a finger touch on port 0 pins (pins 0-7) will slightly change ADC output. Connect port 
   1 (pins 8-15) to the leds on the nRFgo (nRF6310) motherboard to see ADC output. Connect pin 23 to a button on the 
	 nRF6310 motherboard. When no button is pressed, leds will show ADC value from input pin P0.1. When the connected
	 button is pressed, leds will show ADC value from input pin P0.5. */

#include <stdbool.h>
#include "nrf_delay.h"
#include "nrf_gpio.h"

#define INPUT_PIN 23

uint16_t adc_bat = 0;
uint16_t adc_result = 0;


void adc_1()
{
	// interrupt ADC
	NRF_ADC->INTENSET = (ADC_INTENSET_END_Disabled << ADC_INTENSET_END_Pos);						/*!< Interrupt enabled. */
					
	// config ADC
	NRF_ADC->CONFIG	= (ADC_CONFIG_EXTREFSEL_None << ADC_CONFIG_EXTREFSEL_Pos) /* Bits 17..16 : ADC external reference pin selection. */
										| (ADC_CONFIG_PSEL_AnalogInput6 << ADC_CONFIG_PSEL_Pos)					/*!< Use analog input 0 as analog input. */
										| (ADC_CONFIG_REFSEL_VBG << ADC_CONFIG_REFSEL_Pos)							/*!< Use internal 1.2V bandgap voltage as reference for conversion. */
										| (ADC_CONFIG_INPSEL_AnalogInputOneThirdPrescaling << ADC_CONFIG_INPSEL_Pos) /*!< Analog input specified by PSEL with no prescaling used as input for the conversion. */
										| (ADC_CONFIG_RES_10bit << ADC_CONFIG_RES_Pos);									/*!< 10bit ADC resolution. */ 
	
	// enable ADC		
	NRF_ADC->ENABLE = ADC_ENABLE_ENABLE_Enabled;					  													/* Bit 0 : ADC enable. */		
	
	// start ADC conversion
	NRF_ADC->TASKS_START = 1;
	
	// wait for conversion to end
	while (!NRF_ADC->EVENTS_END)
	{}
	NRF_ADC->EVENTS_END	= 0;
	
	//Save your ADC result
	adc_result = NRF_ADC->RESULT;	
		
	//Use the STOP task to save current. Workaround for PAN_028 rev1.1 anomaly 1.
  NRF_ADC->TASKS_STOP = 1;
}


void adc_2()
{
	// interrupt ADC
	NRF_ADC->INTENSET = (ADC_INTENSET_END_Disabled << ADC_INTENSET_END_Pos);						//!< Interrupt enabled. 
					
	// config ADC
	NRF_ADC->CONFIG	= (ADC_CONFIG_EXTREFSEL_None << ADC_CONFIG_EXTREFSEL_Pos) /* Bits 17..16 : ADC external reference pin selection. */
										| (ADC_CONFIG_PSEL_AnalogInput2 << ADC_CONFIG_PSEL_Pos)					/*!< Use analog input 0 as analog input. */
										| (ADC_CONFIG_REFSEL_VBG << ADC_CONFIG_REFSEL_Pos)							/*!< Use internal 1.2V bandgap voltage as reference for conversion. */
										| (ADC_CONFIG_INPSEL_AnalogInputOneThirdPrescaling << ADC_CONFIG_INPSEL_Pos) /*!< Analog input specified by PSEL with no prescaling used as input for the conversion. */
										| (ADC_CONFIG_RES_8bit << ADC_CONFIG_RES_Pos);									/*!< 10bit ADC resolution. */ 
	
	// enable ADC		
	NRF_ADC->ENABLE = ADC_ENABLE_ENABLE_Enabled;					  													// Bit 0 : ADC enable. 	
	
	// start ADC conversion
	NRF_ADC->TASKS_START = 1;
	
	// wait for conversion to end
	while (!NRF_ADC->EVENTS_END)
	{}
	NRF_ADC->EVENTS_END	= 0;

	//Save your ADC result
	adc_bat = NRF_ADC->RESULT;	
		
	//Use the STOP task to save current.
  NRF_ADC->TASKS_STOP = 1;
}

/* main */
int main(void)
{		
	/* Configure pins 8-15 (PORT1) as outputs */
	nrf_gpio_range_cfg_output(8, 15);
	/* Configure input pin for button input. Used to show ADC result on leds for either ADC input pin 2 or ADC input pin 6. */
	nrf_gpio_cfg_input(INPUT_PIN, NRF_GPIO_PIN_NOPULL);
  
	while(true)
	{
		/* Create ADC samples*/
		adc_1(); //Stores ADC sample in adc_result. Converts analog input voltage on pin nr 6 (P0.05)
		adc_2(); //Stores ADC sample in adc_bat.    Converts analog input voltage on pin nr 2 (P0.01)
		
		/* Write ADC result to pins 8-15. If button is pressed adc_result (pin AIN6, P0.05) is shown, otherwise adc_bat (pin AIN2, P0.01) */
		if(!nrf_gpio_pin_read(INPUT_PIN))
			nrf_gpio_port_write(NRF_GPIO_PORT_SELECT_PORT1, adc_result >> 2); 
		else
			nrf_gpio_port_write(NRF_GPIO_PORT_SELECT_PORT1, adc_bat);
		
		nrf_delay_ms(1);
	}
}
