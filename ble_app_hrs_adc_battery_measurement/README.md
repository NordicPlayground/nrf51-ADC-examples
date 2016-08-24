ble_app_uart_adc_battery_measurement example
==================

This project shows nRF51 ADC sampling, example of converting ADC values into battery level, and how battery level value is sent over BLE
 
Requirements
------------
- nRF5 SDK version 11.0.0
- nRF51-DK

To compile it, clone the repository in the \nRF5_SDK_11.0.0_89a8197\examples\ble_peripheral\ folder.  If you download the zip, place the ble_app_uart_adc_button_triggered folder into the \nRF5_SDK_11.0.0_89a8197\examples\ble_peripheral\ folder.

Documentation
-----------------
- Perhipheral: nRF51 ADC
- Compatibility: nRF51 rev 3, nRF5 SDK 11.0.0
- Softdevice used: 130 2.0.x
  
This example is basically the ble_app_uart example in nRF5 SDK 11.0.0 with nRF51 ADC addon functionality. ADC sampling is triggered from an application timer event. The application timer event handler puts ADC sample task into app_scheduler queue, which results in the nrf_drv_adc_sample function to be executed in the main context. 

In this example, a single ADC channel is configured to sample on Analog Input 2, which maps to physical pin P0.01 on the nRF51 IC. Buffer size is set to 6, which makes the nrf_drv_adc driver return callback (function adc_event_handler) first when 6 samples have been triggered with 6 calls to nrf_drv_adc_sample function. The ADC_BUFFER_SIZE constant configures the ADC buffer size. 

The following is performed in the adc_event_handler callback function:
- Average value is calculated from the 6 ADC values
- The average ADC value is transformed into millivolts
- The millivolts value is transformed into battery level procent
- The battery level procent value is sent over BLE

The example outputs the ADC result on UART. The UART output can be seen by connecting with e.g. Realterm UART terminal program for PC (when nRF51-DK connected to PC via USB) with the UART settings configured in the uart_init function, which is also described in the ble_app_uart documentation at http://infocenter.nordicsemi.com/topic/com.nordic.infocenter.sdk5.v11.0.0/ble_sdk_app_nus_eval.html?cp=6_0_0_4_2_2_18_2#project_uart_nus_eval_test
  
Indicators on the nRF51-DK board:
- LED1 blinking: Device advertising
- LED1 on:	Device connected

About this project
------------------
This application is one of several applications that has been built by the support team at Nordic Semiconductor, as a demo of some particular feature or use case. It has not necessarily been thoroughly tested, so there might be unknown issues. It is hence provided as-is, without any warranty. 

However, in the hope that it still may be useful also for others than the ones we initially wrote it for, we've chosen to distribute it here on GitHub. 

The application is built to be used with the official nRF5 SDK, that can be downloaded from http://developer.nordicsemi.com/

Please post any questions about this project on https://devzone.nordicsemi.com.
