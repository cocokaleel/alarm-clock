#include <stdio.h>
#include <stdint.h>
#include <FastLED.h>

/* Function takes in the LED array to light up, the # of LEDs, and
 * the color
 */
void lightLEDs(CRGB* leds, int num_leds, CRGB color) {
  for (int i = 0; i < num_leds; i++) {
    leds[i] = color;
  }
  FastLED.show();
}


void lights2Audio(int* beats[], int num_beats) {
    for (int i = 0; i < num_beats; i++) {
    int curr_time = millis();
    // if the time is within offset bounds
    if (curr_time >= beats[i] - t_offset && curr_time <= beats[i] + t_offset) {
      lightLEDs(leds, NUM_LEDS, color1);
      delayMicroseconds(mils2micros(t_offset));
    } else {
      lightLEDs(leds, NUM_LEDS, color2);
      delayMicroseconds(mils2micros(t_offset));
    }
  }
}

/* *** UNTESTED *** */
void toggle_lights(int* beats[], int num_beats, int bpm, int millis) {
  // calculate the amount of time to turn lights on for (TOGGLE_TIME)
  int TOGGLE_TIME = 1000 / (bpm / 60); // lights should turn from one to off once every TOGGLE_TIME mils
  int TOGGLE_TIME_2 = (( bpm / 2 ) * 1000 * 60); // is this the riight one?
  // TODO: I want toggle time to = bpm / 2, in beats per millis
}


/* helper functtion that converts milliseconds to microseconds that can be used to  
 * make delays with delayMicroseconds() easier
 * this has been manually tested :)
 */
uint32_t mils2micros(uint32_t milliseconds) { // note max input is ~4M, which shouldn't be an issue for our purposes
    return milliseconds * 1000U; // U is added to indicate an unsigned constant
}
