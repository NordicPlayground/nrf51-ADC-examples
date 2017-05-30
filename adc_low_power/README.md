adc_low_power
==================

 This project shows nRF51 ADC sampling tuned for low power
 
Requirements
------------
- nRF5 SDK version 12.2.0
- nRF51-DK

To compile it, clone the repository in the \nRF5_SDK_12.2.0_f012efa\examples\peripheral\ folder.  If you download the zip, place the adc_low_power folder into the \nRF5_SDK_12.2.0_f012efa\examples\peripheral\ folder.

Documentation
-----------------
- Perhipheral: nRF51 ADC
- Compatibility: nRF51 rev 3, nRF5 SDK 12.2.0
- Softdevice used: no softdevice
  
This example shows a way to obtain low current consumption using the nRF51 ADC. To enable lowest current consumption, disable UART logging (NRF_LOGGING_ENABLED) and disable HFCLK crystal while sampling (HIGH_ACCURACY_ADC_ENABLE). This example uses application timer (app_timer) and 32kHz crystal to obtain lowest current consumption.

- Current consumption, 1Hz sampling frequency, HIGH_ACCURACY_ADC_ENABLE=0: 5uA
- Current consumption, 100Hz sampling frequency, HIGH_ACCURACY_ADC_ENABLE=0: 70uA
- Current consumption, 100Hz sampling frequency, HIGH_ACCURACY_ADC_ENABLE=1: 110uA

Functionality: Three ADC channels are sampled. Sampling is triggered on app_timer event. When sampling is triggered all three configured channels are sampled. adc_event_handler is called when 6 samples are collected as adc buffer size is set to 6.

Indicators on the nRF51-DK board:
- nRF51-DK LED 1: Toggles on every app_timer event and adc sampling trigger.
- nRF51-DK LED 2: Toggles on every adc callback, i.e. 6 samples

About this project
------------------
This application is one of several applications that has been built by the support team at Nordic Semiconductor, as a demo of some particular feature or use case. It has not necessarily been thoroughly tested, so there might be unknown issues. It is hence provided as-is, without any warranty. 

However, in the hope that it still may be useful also for others than the ones we initially wrote it for, we've chosen to distribute it here on GitHub. 

The application is built to be used with the official nRF5 SDK, that can be downloaded from http://developer.nordicsemi.com/

Please post any questions about this project on https://devzone.nordicsemi.com.
