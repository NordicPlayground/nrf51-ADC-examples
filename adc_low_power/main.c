/* Copyright (c) 2015 Nordic Semiconductor. All Rights Reserved.
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

/**
 * This example shows a way to obtain low current consumption using the nRF51 ADC.
 * To enabled lowest current consumption, disable UART logging (NRF_LOGGING_ENABLED)
 * and disable HFCLK crystal while sampling (HIGH_ACCURACY_ADC_ENABLE). This example
 * uses application timer (app_timer) and 32kHz crystal to obtain lowest current
 * consumption.
 * Current consumption, 1Hz sampling frequency, HIGH_ACCURACY_ADC_ENABLE=0: 5uA
 * Current consumption, 100Hz sampling frequency, HIGH_ACCURACY_ADC_ENABLE=0: 70uA
 * Current consumption, 100Hz sampling frequency, HIGH_ACCURACY_ADC_ENABLE=1: 110uA
 *
 * Functionality: Three ADC channels are sampled. Sampling is triggered on app_timer
 * event. When sampling is triggered all three configured channels are sampled. 
 * adc_event_handler is called when 6 samples are collected as adc buffer size is 
 * set to 6.
 * nRF51-DK LED 1: Toggles on every app_timer event and adc sampling trigger.
 * nRF51-DK LED 2: Toggles on every adc callback
 */
 
#include "nrf.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "nrf_drv_adc.h"
#include "nordic_common.h"
#include "boards.h"
#include "nrf_log.h"
#include "app_error.h"
#include "app_util_platform.h"
#include "app_timer.h"
#include "nrf_drv_clock.h"

#define HIGH_ACCURACY_ADC_ENABLE        0               //Set to 1 to enable high accuracy ADC sampling (use HFCLK crystal while sampling). Set to 0 for lowest current consumption.
#define NRF_LOGGING_ENABLED             0               //Set as 1 to enable logging on UART, otherwise set as 0. Comment out to enable low current consumption
#define ADC_BUFFER_SIZE                 6               //Size of buffer for ADC samples. Buffer size should be multiple of number of adc channels located.

#define APP_TIMER_PRESCALER             0                                          /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_OP_QUEUE_SIZE         4                                          /**< Size of timer operation queues. */
#define TIMER1_INTERVAL                 APP_TIMER_TICKS(10, APP_TIMER_PRESCALER) /**< Defines the interval between consecutive app timer interrupts in milliseconds. */

static nrf_adc_value_t         adc_buffer[ADC_BUFFER_SIZE]; /**< ADC buffer. */
static uint8_t adc_event_counter = 0;
static uint8_t number_of_adc_channels;

APP_TIMER_DEF(m_timer1_id); 

/**
 * @brief ADC interrupt handler.
 */
static void adc_event_handler(nrf_drv_adc_evt_t const * p_event)
{
    if(HIGH_ACCURACY_ADC_ENABLE)
    {
        nrf_drv_clock_hfclk_release();
    }
	
    if (p_event->type == NRF_DRV_ADC_EVT_DONE)
    {
        if(NRF_LOGGING_ENABLED)
        {					
            uint32_t i;
            NRF_LOG_PRINTF("  adc event counter: %d\r\n", adc_event_counter);
            for (i = 0; i < p_event->data.done.size; i++)
            {
                NRF_LOG_PRINTF("ADC value channel %d: %d\r\n", (i % number_of_adc_channels), p_event->data.done.p_buffer[i]);
            }
            adc_event_counter++;
        }

        APP_ERROR_CHECK(nrf_drv_adc_buffer_convert(adc_buffer,ADC_BUFFER_SIZE));
        LEDS_INVERT(BSP_LED_1_MASK);
    }
}

/**
 * @brief ADC initialization.
 */
static void adc_config(void)
{
    ret_code_t ret_code;
	
    //Initialize ADC
    nrf_drv_adc_config_t config = NRF_DRV_ADC_DEFAULT_CONFIG;
    ret_code = nrf_drv_adc_init(&config, adc_event_handler);
    APP_ERROR_CHECK(ret_code);
	
    //Configure and enable ADC channel 0
    static nrf_drv_adc_channel_t m_channel_0_config = NRF_DRV_ADC_DEFAULT_CHANNEL(NRF_ADC_CONFIG_INPUT_2); 
    m_channel_0_config.config.config.input = NRF_ADC_CONFIG_SCALING_INPUT_ONE_THIRD;
    nrf_drv_adc_channel_enable(&m_channel_0_config);
	
    //Configure and enable ADC channel 1
    static nrf_drv_adc_channel_t m_channel_1_config = NRF_DRV_ADC_DEFAULT_CHANNEL(NRF_ADC_CONFIG_INPUT_6); 
    m_channel_1_config.config.config.input = NRF_ADC_CONFIG_SCALING_INPUT_ONE_THIRD;
    nrf_drv_adc_channel_enable(&m_channel_1_config);
	
    //Configure and enable ADC channel 2
    static nrf_drv_adc_channel_t m_channel_2_config = NRF_DRV_ADC_DEFAULT_CHANNEL(NRF_ADC_CONFIG_INPUT_7);	
    m_channel_2_config.config.config.input = NRF_ADC_CONFIG_SCALING_INPUT_ONE_THIRD;
    nrf_drv_adc_channel_enable(&m_channel_2_config);
	
    number_of_adc_channels = 3;    //Set equal to the number of configured ADC channels, for the sake of UART output.
}

static void timer1_handler(void * p_context)
{
    LEDS_INVERT(BSP_LED_0_MASK);                                                                //Indicate execution of timer1 interrupt handler on LED1
    if(HIGH_ACCURACY_ADC_ENABLE)
    {
        nrf_drv_clock_hfclk_request(NULL);
        while(!nrf_drv_clock_hfclk_is_running());
    }
    nrf_drv_adc_sample();
}


static void timers_init(void)
{
    uint32_t err_code;

    // Initialize timer module.
    APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, false);

    // Create timers.
    err_code = app_timer_create(&m_timer1_id,
                                APP_TIMER_MODE_REPEATED,
                                timer1_handler);
    APP_ERROR_CHECK(err_code);
}

static void application_timers_start(void)
{
    uint32_t err_code;

    // Start application timers.
    err_code = app_timer_start(m_timer1_id, TIMER1_INTERVAL, NULL);
    APP_ERROR_CHECK(err_code);
}

static void clock_init(void)
{
    uint32_t err_code;
	
    err_code = nrf_drv_clock_init();
    APP_ERROR_CHECK(err_code);
    nrf_drv_clock_lfclk_request(NULL);
    while(!nrf_drv_clock_lfclk_is_running());                                    //Wait until the lfclk is running
}

/**
 * @brief Function for main application entry.
 */
int main(void)
{
    LEDS_CONFIGURE(LEDS_MASK);
    LEDS_OFF(LEDS_MASK);

    adc_config();
    clock_init();
    timers_init();
    application_timers_start();
	
    if(NRF_LOGGING_ENABLED)
    {
        NRF_LOG_INIT();
        NRF_LOG_PRINTF("    ADC example\r\n");
    }
	
    APP_ERROR_CHECK(nrf_drv_adc_buffer_convert(adc_buffer,ADC_BUFFER_SIZE));
	
    while (true)
    {
            __WFE();    
            __SEV();
            __WFE();
    }
}
/** @} */
