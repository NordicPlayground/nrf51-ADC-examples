/* Copyright (c) 2009 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */

#include <stdbool.h>
#include "nrf.h"
#include "nrf_gpio.h"
#define NRF_LOG_MODULE_NAME "APP"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "app_error.h"
#include "app_util_platform.h"

#define LFCLK_FREQUENCY           (32768UL)                             /*!< LFCLK frequency in Hertz, constant */
#define RTC_FREQUENCY             (8UL)                                 /*!< required RTC working clock RTC_FREQUENCY Hertz. Changable */
#define COUNTER_PRESCALER         ((LFCLK_FREQUENCY/RTC_FREQUENCY) - 1) /* f = LFCLK/(prescaler + 1) */

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
	NRF_ADC->CONFIG	= (ADC_CONFIG_EXTREFSEL_None << ADC_CONFIG_EXTREFSEL_Pos) 								    /* Bits 17..16 : ADC external reference pin selection. */
										| (ADC_CONFIG_PSEL_AnalogInput6 << ADC_CONFIG_PSEL_Pos)					/*!< Use analog input 6 as analog input (P0.05). */
										| (ADC_CONFIG_REFSEL_VBG << ADC_CONFIG_REFSEL_Pos)						/*!< Use internal 1.2V bandgap voltage as reference for conversion. */
										| (ADC_CONFIG_INPSEL_AnalogInputNoPrescaling << ADC_CONFIG_INPSEL_Pos) 	/*!< Analog input specified by PSEL with no prescaling used as input for the conversion. */
										| (ADC_CONFIG_RES_10bit << ADC_CONFIG_RES_Pos);							/*!< 10bit ADC resolution. */ 	
	
	/* Enable ADC*/
	NRF_ADC->ENABLE = ADC_ENABLE_ENABLE_Enabled;	
}

/* Interrupt handler for ADC data ready event. It will be executed when ADC sampling is complete */
void ADC_IRQHandler(void)
{
	/* Clear dataready event */
    NRF_ADC->EVENTS_END = 0;	

	//Print ADC result on UART
	NRF_LOG_INFO("sample result: %X\r\n", NRF_ADC->RESULT);
}


/**
 * main function
 */
int main(void)
{	
	APP_ERROR_CHECK(NRF_LOG_INIT(NULL));    //NRF_LOG documentation: http://infocenter.nordicsemi.com/topic/com.nordic.infocenter.sdk5.v11.0.0/lib_nrf_log.html?cp=4_0_1_3_33
	NRF_LOG_INFO("    ADC example - no drivers, interrupt driven\r\n");
	
	hfclk_config();                         //Enable 16MHz crystal for maximum ADC accuracy. Comment out this line to use internal 16MHz RC instead, which saves power.
	lfclk_config();			                //Configure 32kHz clock, required by the RTC timer
    rtc_config();				            //Configure RTC timer
	ppi_init();					            //Configure PPI channel, which connects RTC TICK event and ADC START task
	ADC_init();					            //Configure the ADC
	
    while (true)
    {	
        //Put CPU to sleep while waiting for interrupt
        __WFI();
        NRF_LOG_FLUSH();
    }
}
/** @} */
