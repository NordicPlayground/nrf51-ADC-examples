ble_app_uart_adc_simple example
==================

 This project shows basic nRF51 ADC functionality
 
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
  
This example is basically the ble_app_uart example in nRF5 SDK 11.0.0 with nRF51 ADC addon functionality. The example works as follows:
- Softdevice and modules are initialized, UART is set up, adc is configured to sample on analog input pin AIN2 (P0.01). ADC buffer size is set to 4 samples.
- ADC samples 4 times a second (see main loop). Code execution is in busy waiting loop (with calling nrf_delay_ms) while not sampling.
- When ADC buffer is full, the nrf_drv_adc driver calls the application in function adc_event_handler and the ADC values are printed on UART.

The UART output can be seen by connecting with e.g. Realterm UART terminal program for PC (when nRF51-DK connected to PC via USB) with the UART settings configured in the uart_init function, which is also described in the ble_app_uart documentation at http://infocenter.nordicsemi.com/topic/com.nordic.infocenter.sdk5.v11.0.0/ble_sdk_app_nus_eval.html?cp=6_0_0_4_2_2_18_2#project_uart_nus_eval_test
  
Indicators on the nRF51-DK board:
- LED1 blinking: Device advertising
- LED1 on:	Device connected
- LED2 toggle: Trigger sampling
- LED4 toggle: ADC sampling complete and ADC buffer full, i.e. 4 samples

About this project
------------------
This application is one of several applications that has been built by the support team at Nordic Semiconductor, as a demo of some particular feature or use case. It has not necessarily been thoroughly tested, so there might be unknown issues. It is hence provided as-is, without any warranty. 

However, in the hope that it still may be useful also for others than the ones we initially wrote it for, we've chosen to distribute it here on GitHub. 

The application is built to be used with the official nRF5 SDK, that can be downloaded from http://developer.nordicsemi.com/

Please post any questions about this project on https://devzone.nordicsemi.com.
