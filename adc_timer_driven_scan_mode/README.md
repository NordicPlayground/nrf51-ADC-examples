adc_timer_driven_scan_mode example
==================

 This project shows nRF51 ADC sampling in scan mode
 
Requirements
------------
- nRF5 SDK version 11.0.0
- nRF51-DK

To compile it, clone the repository in the \nRF5_SDK_11.0.0_89a8197\examples\peripheral\ folder.  If you download the zip, place the ble_app_uart_adc_button_triggered folder into the \nRF5_SDK_11.0.0_89a8197\examples\peripheral\ folder.

Documentation
-----------------
- Perhipheral: nRF51 ADC
- Compatibility: nRF51 rev 3, nRF5 SDK 11.0.0
- Softdevice used: no softdevice
  
TIMER2 peripheral periodically generates events and triggers the ADC START task through a PPI channel. 

In this example, three ADC channels are configured to enable scan mode. For a single ADC START task, the ADC samples once on each channel. The three configured channels sample on analog input pins AIN2 (P0.01), AIN6 (P0.05) and AIN7 (P0.06). Result is output on UART. Buffer size is set to 6, which makes the nrf_drv_adc driver return callback first when ADC START task has been triggered twice, since three channels are configured. 

The UART output can be seen by connecting with e.g. Realterm UART terminal program for PC (when nRF51-DK connected to PC via USB) with the UART settings configured in the uart_init function, which is also described in the ble_app_uart documentation at http://infocenter.nordicsemi.com/topic/com.nordic.infocenter.sdk5.v11.0.0/ble_sdk_app_nus_eval.html?cp=6_0_0_4_2_2_18_2#project_uart_nus_eval_test
  
Indicators on the nRF51-DK board:
- LED1 toggle: ADC sampling complete and ADC buffer full, i.e. 6 samples

About this project
------------------
This application is one of several applications that has been built by the support team at Nordic Semiconductor, as a demo of some particular feature or use case. It has not necessarily been thoroughly tested, so there might be unknown issues. It is hence provided as-is, without any warranty. 

However, in the hope that it still may be useful also for others than the ones we initially wrote it for, we've chosen to distribute it here on GitHub. 

The application is built to be used with the official nRF5 SDK, that can be downloaded from http://developer.nordicsemi.com/

Please post any questions about this project on https://devzone.nordicsemi.com.
