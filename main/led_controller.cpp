/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "bt_app_av.h"
#include "FastLED.h"
#include "FX.h"

// LED Settings

volatile uint32_t gVariable = 97;
volatile uint32_t gBrightness = 100;
volatile uint32_t gColor = 200;
volatile uint32_t gSpeed = 5;
volatile uint32_t gSpeedPulse = 5;
volatile uint32_t gFFTrefresh = 75;


volatile bool pulse = false;
volatile bool goUp = false;
volatile bool goDown = false;

#define LED_COUNT   64
#define LED_PIN     32
#define CHANNEL     0

#define NUM_LEDS 64
#define DATA_PIN 13

// FFT Settings
#define NUM_BANDS  8
#define SAMPLES 2048              

#define BRIGHTNESS 10


CRGB leds[NUM_LEDS];
CHSV leds_HSV(0,255,255);

int pushButton = 39;

float amplitude = 200.0;

int32_t peak[] = {0, 0, 0, 0, 0, 0, 0, 0};


double brightness = 0.25;

QueueHandle_t queue;

int16_t sample_l_int;
int16_t sample_r_int;

uint32_t animationCounter = 0;

int visualizationCounter = 0;
int32_t lastVisualizationUpdate = 0;


uint8_t hueOffset = 0;

bool hasDevicePlayedAudio = false;

uint8_t getLedIndex(uint8_t x, uint8_t y) {
  //x = 7 - x;

    return y * 8 + x;

}

void createBands(int i, int dsize) {
  uint8_t band = 0;
  if (i <= 2) {
    band =  0; // 125Hz
  } else if (i <= 6) {
    band =   1; // 250Hz
  } else if (i <= 12)  {
    band =  2; // 500Hz
  } else if (i <= 24) {
    band =  3; // 1000Hz
  } else if (i <= 46) {
    band =  4; // 2000Hz
  } else if (i <= 93) {
    band =  5; // 4000Hz
  } else if (i <= 372) {
    band =  6;// 8000Hz
  } else {
    band = 7;
  }
  int dmax = amplitude;
  if (dsize > dmax)
    dsize = dmax;
  if (dsize > peak[band])
  {
    peak[band] = dsize;
  }
//  if(band == 0){
//   printf("BAND === %d |||||| BandNum %d  |||||| datasize %d \n" ,peak[band],band, dsize);
//  }
}
extern "C"  void drawColor(){
        for(int i = 0 ; i < NUM_LEDS; i++){
          leds_HSV.hue = gColor;
          hsv2rgb_rainbow(leds_HSV,leds[i]);
        }
}
extern "C"  void renderFFT(float * y1_cf){
  
  int item = 2;


      for (uint8_t band = 0; band < NUM_BANDS; band++) {
        peak[band] = 0;
      }

      for (int i = 2; i < (SAMPLES / 2); i++) { // Don't use sample 0 and only first SAMPLES/2 are usable. Each array eleement represents a frequency and its value the amplitude.
        
          int value = ((int)y1_cf[i]+60);
          //printf("sample %f", y1_cf[i]);
          createBands(i,  value);
      }
      
      uint8_t intensity;
      
      FastLED.clear();
      FastLED.setBrightness(gBrightness);
      for (byte band = 0; band < NUM_BANDS; band++) {
        intensity = map8(peak[band], 0, 60);
 //       printf("intensity  %d   |||| %d Band \n",intensity,band);

 
        switch(gVariable){
            case 97:
              for (int i = 0; i < 8; i++) {
                  leds[getLedIndex( i,  band)] = (i >= intensity) ? CHSV(0, 0, 0) : CHSV(i * 16, 255, 255);
                }
              break;
            case 100:
              for (int i = 0; i < 8; i++) {
                  leds[getLedIndex( i,  band)] = (i >= intensity) ? CHSV(0, 0, 0) : CHSV(gColor, 255, 255);
                }
              break;
            case 101:
                for (int i = 0; i < 8; i++) {
                  leds[getLedIndex( i,  band)] = (i >= intensity) ? CRGB::Black : CRGB::Green;
                }
              break;
            case 103:
                for (int i = 0; i < 8; i++) {
                  leds[getLedIndex( i,  band)] = (i >= intensity) ? CRGB::Black : CRGB::Blue;
                }
              break;
            default: printf("Not good case %d", gVariable); gVariable = 97;
        
        }
      }

      FastLED.show();

      if ((millis() - lastVisualizationUpdate) > 1000) {
        log_e("Fps: %f", visualizationCounter / ((millis() - lastVisualizationUpdate) / 1000.0));
        visualizationCounter = 0;
        lastVisualizationUpdate = millis();
        hueOffset += 5;
      }
      visualizationCounter++;
  
}
extern "C" const uint32_t HEART[] = {
    0x00222222, 0x00222222, 0x0000ff00, 0x0000ff00, 0x0000ff00, 0x00222222, 0x00222222, 0x00222222,
    0x0000ff00, 0x0000ff00, 0x0000ff00, 0x0000ff00, 0x0000ff00, 0x0000ff00, 0x00222222, 0x00222222,
    0x0000ff00, 0x0000ff00, 0x0000ff00, 0x0000ff00, 0x0000ff00, 0x0000ff00, 0x0000ff00, 0x00222222,
    0x00222222, 0x00222222, 0x0000ff00, 0x0000ff00, 0x0000ff00, 0x0000ff00, 0x0000ff00, 0x0000ff00,
    0x00222222, 0x00222222, 0x0000ff00, 0x0000ff00, 0x0000ff00, 0x0000ff00, 0x0000ff00, 0x0000ff00,
    0x0000ff00, 0x0000ff00, 0x0000ff00, 0x0000ff00, 0x0000ff00, 0x0000ff00, 0x0000ff00, 0x00222222,
    0x0000ff00, 0x0000ff00, 0x0000ff00, 0x0000ff00, 0x0000ff00, 0x0000ff00, 0x00222222, 0x00222222,
    0x00222222, 0x00222222, 0x0000ff00, 0x0000ff00, 0x0000ff00, 0x00222222, 0x00222222, 0x00222222};
extern "C"  void drawHeart(){
        for(int i = 0 ; i < NUM_LEDS; i++){
          leds[i]=HEART[i];
        }
}

extern "C"  void rainbowMarch(){

        uint8_t thisHue = beat8(gSpeed,255);                     // A simple rainbow march
        fill_rainbow(leds, NUM_LEDS, thisHue, 1);            // Use FastLED's fill_rainbow routine.                                    // Speed, delta hue values.
     
}
extern "C"  void rainbowWave(){

        uint8_t thisHue = beatsin8(gSpeed,0,255);                     // A simple rainbow march
        fill_rainbow(leds, NUM_LEDS, thisHue, 1);            // Use FastLED's fill_rainbow routine.                                    // Speed, delta hue values.
     
}

 extern "C" void drawIcon(void * p_param) {
  while (1)
  {
    if(pulse){
      if(gBrightness > 80){
        gBrightness = 80;
        goUp = false;
      }
      if(gBrightness < 5){
        gBrightness = 5;
        goUp = true;
      }
      if(gBrightness == 80){
        goUp = false;
      }
      else if(gBrightness == 5){
        goUp = true;
      }


      if(goUp == true)
      {
      gBrightness++;
      }
      else{
      gBrightness--;
      }
    vTaskDelay(gSpeedPulse/ portTICK_PERIOD_MS);   
    }
    FastLED.setBrightness(gBrightness);  
    vTaskDelay(10 / portTICK_PERIOD_MS);   


    switch(gVariable){
      case 97: rainbowMarch(); break;
      case 98: rainbowWave(); break;
      case 99: drawHeart(); pulse = false; break;
      case 100: drawColor(); break;
      case 101: 
                if(!pulse)
                {goUp = true;gBrightness =10; pulse = true;}   
                 rainbowMarch();  break;
      case 102: 
                if(pulse)
            {goUp = false;gBrightness =10; pulse = false;}   
              rainbowWave();  break;

    }   
    FastLED.show();
    vTaskDelay(10/ portTICK_PERIOD_MS);   
  }
}

extern "C"  void clear_leds(){

      FastLED.clear();
}
extern "C"  void setup_leds(){

    FastLED.addLeds<WS2812B, DATA_PIN>(leds, NUM_LEDS);
    FastLED.setMaxPowerInVoltsAndMilliamps(12,3000);

}