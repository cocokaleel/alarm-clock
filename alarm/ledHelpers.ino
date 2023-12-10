

/*
 * sets up the LEDs 
 */
void setupLEDs(int brightness) {
    FastLED.setBrightness(  BRIGHTNESS );
    Serial.println("successfully initialized LEDs");
}

/*
 * Main LED function that is called in alarm.ino
 * The rest of this file contains helper functions for this
 */
void ledParty(CRGB* leds, int num_leds, CRGB color1, CRGB color2, int bpm) {
  Serial.println("running ledParty()");
  int beats_in_song = 60 * 4; // assume each song is 4 mins

  // calculate delay time based on BPM
  int delay_time = (60 * 1000 / (bpm * 2));

  // for the length of the song. flash between colors according to bpm
  for (int i = 0; i < beats_in_song; i++) {
    lightLEDs(leds, num_leds, color1); 
    delay(delay_time);
    lightLEDs(leds, num_leds, color2);
    delay(delay_time);
  }
}

/* 
 * helper functtion that converts milliseconds to microseconds that can be used to  
 * make delays with delayMicroseconds() easier, since delay() doesn't work w ISRs
 * this has been manually tested :)
 */
uint32_t mils2micros(uint32_t milliseconds) { // note max input is ~4M, which shouldn't be an issue for our purposes
    return milliseconds * 1000U; // U is added to indicate an unsigned constant
}

/* Function takes in the LED array to light up, the # of LEDs, and
 * the color
 */
void lightLEDs(CRGB* leds, int num_leds, CRGB color) {
  for (int i = 0; i < num_leds; i++) {
    leds[i] = color;
  }
  FastLED.show();
}