#include "esp_dsp.h"
#include <stdint.h>
#include "esp_log.h"
#include "bt_app_core.h"

#include "driver/i2s.h"
#include "freertos/ringbuf.h"
#include "led_controller.hpp"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "esp_log.h"

#include "esp_dsp.h"
#include "fft_controller.h"
#include "bt_app_av.h"

#define N_SAMPLES 2048


bool fft_running;
float *x1, *wind, *y_cf, *y1_cf, *y2_cf;
uint8_t fft_buffer[N_SAMPLES * 2]; // 4096 because that's the length of the esp-adf's a2dp callback buffer
static const char *TAG = "FFT DATA REV";





void init_fft()
{
  esp_err_t ret = dsps_fft2r_init_fc32(NULL, CONFIG_DSP_MAX_FFT_SIZE);
  x1 = (float *)calloc(N_SAMPLES, sizeof(float));
  wind = (float *)calloc(N_SAMPLES, sizeof(float));
  y_cf = (float *)calloc(N_SAMPLES * 2, sizeof(float));
  y1_cf = &y_cf[0];
  y2_cf = &y_cf[0];
  dsps_wind_hann_f32(wind, N_SAMPLES);
}
extern uint8_t globalIntVar;

void calculate_fft()
{


  while(1){

  fft_running = true; 


    int t = 0;
    static int16_t sample_l_int = 0;
    static int16_t sample_r_int = 0;
    static float sample_l_float = 0.0f;
    static float sample_r_float = 0.0f;
    static float in = 0.0f;

    for (uint32_t i = 0; i < N_SAMPLES; i += 4)
    {
      sample_l_int = (int16_t)((*(fft_buffer + i + 1) << 8) | *(fft_buffer + i));
      sample_r_int = (int16_t)((*(fft_buffer + i + 3) << 8) | *(fft_buffer + i + 2));
      sample_l_float = (float)sample_l_int / 0x8000;
      sample_r_float = (float)sample_r_int / 0x8000;
      in = (sample_l_float + sample_r_float) / 2.0f;
      x1[t] = in * wind[t];
      t++;
    }


  for (int i = 0; i < N_SAMPLES; i++)
  {
    y_cf[i * 2 + 0] = x1[i] * wind[i];
    y_cf[i * 2 + 1] = 0;
  }


    dsps_fft2r_fc32(y_cf, N_SAMPLES);
    // Bit reverse
    dsps_bit_rev_fc32(y_cf, N_SAMPLES);

    for (int i = 0; i < N_SAMPLES / 2; i++)
    {
      y1_cf[i] = 10 * log10f((y1_cf[i * 2 + 0] * y1_cf[i * 2 + 0] + y1_cf[i * 2 + 1] * y1_cf[i * 2 + 1]) / N_SAMPLES);
    }

    // dsps_view(y1_cf, N_SAMPLES / 2, 128, 20, -60, 40, '|');
    renderFFT(y1_cf);

  fft_running = false; 
  vTaskDelay(65/ portTICK_PERIOD_MS); 
  }
}

  void copy_a2dp_buffer_for_fft(const uint8_t *data, uint32_t len)
  {
    if (!fft_running) {
      memcpy(fft_buffer, data, len);
    }
  }

#ifdef __cplusplus
}
#endif