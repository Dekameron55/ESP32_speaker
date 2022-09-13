#ifndef __FFT_CONTROLLER__
#define __FFT_CONTROLLER__

void init_fft();
void calculate_fft();
void copy_a2dp_buffer_for_fft(const uint8_t *data, uint32_t len);
extern bool fft_running;


#endif /* __FFT_CONTROLLER__*/