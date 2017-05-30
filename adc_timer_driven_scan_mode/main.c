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

/** @file
 * @defgroup nrf_adc_example main.c
 * @{
 * @ingroup nrf_adc_example
 * @brief ADC Example Application main file.
 *
 * This file contains the source code for a sample application using the ADC driver.
 */

#include "nrf.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "nrf_drv_adc.h"
#include "nordic_common.h"
#include "boards.h"
#define NRF_LOG_MODULE_NAME "APP"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "app_error.h"
#include "app_util_platform.h"
#include "nrf_ppi.h"
#include "nrf_drv_timer.h"
#include "nrf_drv_ppi.h"


#define ADC_BUFFER_SIZE 6       //Size of buffer for ADC samples. Buffer size should be multiple of number of adc channels located.
#define ADC_SAMPLE_RATE	1000     //Sets the sampling rate in ms

static nrf_adc_value_t          adc_buffer[ADC_BUFFER_SIZE]; /**< ADC buffer. */
static nrf_ppi_channel_t        m_ppi_channel;
static const nrf_drv_timer_t    m_timer = NRF_DRV_TIMER_INSTANCE(2);
static uint8_t                  adc_event_counter = 0;
static uint8_t                  number_of_adc_channels;

/**
 * @brief ADC interrupt handler.
 */
static void adc_event_handler(nrf_drv_adc_evt_t const * p_event)
{
    if (p_event->type == NRF_DRV_ADC_EVT_DONE)
    {
        uint32_t i;
        NRF_LOG_INFO("  adc event counter: %d\r\n", adc_event_counter);
        for (i = 0; i < p_event->data.done.size; i++)
        {
            NRF_LOG_INFO("ADC value channel %d: %d\r\n", (i % number_of_adc_channels), p_event->data.done.p_buffer[i]);
        }
    adc_event_counter++;
    }
    APP_ERROR_CHECK(nrf_drv_adc_buffer_convert(adc_buffer,ADC_BUFFER_SIZE));

    bsp_board_led_invert(BSP_BOARD_LED_0);
}

void timer_handler(nrf_timer_event_t event_type, void* p_context)
{
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

void adc_sampling_event_enable(void)
{
    ret_code_t err_code = nrf_drv_ppi_channel_enable(m_ppi_channel);
    APP_ERROR_CHECK(err_code);
}

void adc_sampling_event_init(void)
{
    ret_code_t err_code;
    err_code = nrf_drv_ppi_init();
    APP_ERROR_CHECK(err_code);

    nrf_drv_timer_config_t timer_config = NRF_DRV_TIMER_DEFAULT_CONFIG;
    timer_config.frequency = NRF_TIMER_FREQ_31250Hz;
    err_code = nrf_drv_timer_init(&m_timer, &timer_config, timer_handler);
    APP_ERROR_CHECK(err_code);

    /* setup m_timer for compare event */
    uint32_t ticks = nrf_drv_timer_ms_to_ticks(&m_timer,ADC_SAMPLE_RATE);
    nrf_drv_timer_extended_compare(&m_timer, NRF_TIMER_CC_CHANNEL0, ticks, NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, true);
    nrf_drv_timer_enable(&m_timer);

    uint32_t timer_compare_event_addr = nrf_drv_timer_compare_event_address_get(&m_timer, NRF_TIMER_CC_CHANNEL0);
    uint32_t adc_sample_event_addr = nrf_drv_adc_start_task_get();

    /* setup ppi channel so that timer compare event is triggering sample task in SAADC */
    err_code = nrf_drv_ppi_channel_alloc(&m_ppi_channel);
    APP_ERROR_CHECK(err_code);
    
    err_code = nrf_drv_ppi_channel_assign(m_ppi_channel, timer_compare_event_addr, adc_sample_event_addr);  //NRF_ADC->TASKS_START);
    APP_ERROR_CHECK(err_code);
}


/**
 * @brief Function for main application entry.
 */
int main(void)
{
    bsp_board_leds_init();

    adc_sampling_event_init();
    adc_config();
    adc_sampling_event_enable();
    APP_ERROR_CHECK(NRF_LOG_INIT(NULL));

    NRF_LOG_INFO("    ADC example\r\n");

    APP_ERROR_CHECK(nrf_drv_adc_buffer_convert(adc_buffer,ADC_BUFFER_SIZE));
	
    while (true)
    {
        __WFE();    
        __SEV();
        __WFE();
        NRF_LOG_FLUSH();
    }
}
/** @} */
