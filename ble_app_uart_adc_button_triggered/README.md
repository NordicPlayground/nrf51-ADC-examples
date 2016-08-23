ble_app_uart_adc_button_triggered example
==================

 This project shows button triggered nRF51 ADC sampling
 
Requirements
------------
- nRF5 SDK version 11.0.0
- nRF51-DK

To compile it, clone the repository in the \nRF5_SDK_11.0.0_89a8197\examples\peripheral\ folder.  If you download the zip, place the saadc_low_power folder into the \nRF5_SDK_11.0.0_89a8197\examples\peripheral\ folder.

Documentation
-----------------
- Perhipheral: nRF51 ADC
- Compatibility: nRF51 rev 3, nRF5 SDK 11.0.0
- Softdevice used: 130 2.0.x
  
This example is basically the ble_app_uart example in nRF5 SDK 11.0.0 with nRF51 ADC addon functionality. ADC sampling is triggered with pressing Button 4 (search for BSP_EVENT_KEY_3 in this file) on the nRF51-DK board. When Button 4 is pressed, ADC sample task is put into app_scheduler queue, which results in the adc_sample function to be executed in the main context. The ADC outputs the result on UART.

In this example, a single ADC channel is configured to sample on Analog Input 2, which maps to physical pin P0.01 on the nRF51 IC. Buffer size is set to 3, which makes the nrf_drv_adc driver return callback first when three samples have been triggered with three calls to nrf_drv_adc_sample function. When Button 4 is pressed once, buffer is allocated and the nrf_drv_adc_sample funciton is called three times which will output three ADC sample results on the UART. The ADC_BUFFER_SIZE constant configures the ADC buffer size. 

The UART output can be seen by connecting with e.g. Realterm UART terminal program for PC (when nRF51-DK connected to PC via USB) with the UART settings configured in the uart_init function, which is also described in the ble_app_uart documentation at http://infocenter.nordicsemi.com/topic/com.nordic.infocenter.sdk5.v11.0.0/ble_sdk_app_nus_eval.html?cp=6_0_0_4_2_2_18_2#project_uart_nus_eval_test
  
Indicators on the nRF52-DK board:
- LED1 blinking: Device advertising
- LED1 on:	Device connected
- LED4 toggle: ADC sampling complete and ADC buffer full

About this project
------------------
This application is one of several applications that has been built by the support team at Nordic Semiconductor, as a demo of some particular feature or use case. It has not necessarily been thoroughly tested, so there might be unknown issues. It is hence provided as-is, without any warranty. 

However, in the hope that it still may be useful also for others than the ones we initially wrote it for, we've chosen to distribute it here on GitHub. 

The application is built to be used with the official nRF5 SDK, that can be downloaded from http://developer.nordicsemi.com/

Please post any questions about this project on https://devzone.nordicsemi.com.
