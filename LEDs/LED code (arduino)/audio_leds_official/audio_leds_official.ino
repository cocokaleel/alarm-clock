/// @file    Blink.ino
/// @brief   Blink the first LED of an LED strip
/// @example Blink.ino

#include <FastLED.h>
#include "led_helpers.h"

#define NUM_LEDS 300 // # of LEDs in your strip
#define DATA_PIN 6
#define BRIGHTNESS  4 // 16 is a good value for this, was 64 in example code (max: 255)


int bpm = 60;
int t_offset = 500; // light lights for 400ms total
int num_beats = 1000;
int beats[] = {1000, 3000, 5000, 7000, 9000, 11000, 13000, 15000, 17000};
// panama beats

// Define the array of leds
CRGB leds[NUM_LEDS];
CRGB color1 = CRGB::Purple;
CRGB color2 = CRGB::Red;

void setup() { 
    FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS);  // GRB ordering is typical  
    FastLED.setBrightness(  BRIGHTNESS );

}

void loop() { 

  int start_time = millis();

  // void lightLEDs(CRGB* LEDarray[], int NUM_LEDS, char* color1)
  Serial.print("starting loop at ");
  Serial.println(start_time);
  Serial.println();

  int beats_idx = 0;

    // while (beats_idx < num_beats):
    //   curr_time = millis();
    //   if (curr_time >= beats[beats_idx] - t_offset && curr_time <= beats[beats_idx] + t_offset) {
    //     lightLEDs(leds, NUM_LEDS, color1);
    //   } else {
    //     lightLEDs(leds, NUM_LEDS, color2);
    //   }
    //   beats_idx++;
    

  for (int i = 0; i < num_beats; i++) {
    int curr_time = millis();
    // if the time is within offset bounds
    if (curr_time >= beats[i] - t_offset && curr_time <= beats[i] + t_offset) {
      lightLEDs(leds, NUM_LEDS, color1);
      // delayMicroseconds(mils2micros(t_offset));
    } else {
      lightLEDs(leds, NUM_LEDS, color2);
    }
  }



  /*
  lightLED(leds, NUM_LEDS, color1);
  delay(2000);
  lightLED(leds, NUM_LEDS, color2);
  delay(2000);
  */

  
}