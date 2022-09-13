#ifndef __LED_CONTROLLER__
#define __LED_CONTROLLER__

void renderFFT(float * y1_cf);
void setup_leds();
void drawIcon();
void clear_leds();
extern volatile uint32_t gVariable;
extern volatile uint32_t gBrightness;
extern volatile uint32_t gColor;
extern volatile uint32_t gSpeed;
extern volatile uint32_t gSpeedPulse;
extern volatile uint32_t gFFTrefresh;
;



#endif /* __FFT_CONTROLLER__*/