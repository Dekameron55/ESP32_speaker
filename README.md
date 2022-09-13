
ESP-IDF A2DP demo
======================

Demo of A2DP audio sink role for the ESP32 based on an example by Espressif.

The device can be controlled via this app:
https://github.com/Dekameron55/RC_tooth

Used HW for the project is 8x LED ws2812 1x ESP32 module 1x tms320f28379d dac and 1x audio amplifier PAM8403.

The application receives incoming PCM data via bluetooth, performs FFT on the data and renders the FFT data on the LED matrix, while sending music data via I2C to the DAC and playing sound via the output of the amplifier, fed by the DAC.

The device lightning animation can be controlled in parallel as well by an Android aplication.
It supports the following features:
-screen saver when music is not playing.
-color change for the led matrix.
-brightness adjustment.

![Screenshot](https://github.com/Dekameron55/ESP32_A2DP_sink_mod/blob/main/Photo1.jpg)

When the device is active.

![Screenshot](https://github.com/Dekameron55/ESP32_A2DP_sink_mod/blob/main/Photo2.jpg)
