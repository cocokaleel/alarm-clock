

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
void ledParty(CRGB* leds, int num_leds, CRGB color, int bpm) {
  Serial.println("running ledParty()");
  // used to calculate how many times LEDs flash over course of song / func
  int beats_in_song = 60 * 4; // assume each song is 4 mins

  // calculate delay time based on BPM
  int delay_time = ((60 / (bpm * 2)) * 500);
  Serial.print("curr time before LED party for loop: ");
  Serial.println(millis());
  int idx = 0;
  // while ( idx < beats_in_song) {
  //   lightLEDs(leds, 50, CRGB::Blue);
  //   delay(delay_time);
  //   // delayMicroseconds(mils2micros(delay_time));
  //   lightLEDs(leds, 50, CRGB::Red);
  //   delay(delay_time);
  //   Serial.print("idx: ");
  //   Serial.print(idx);
  //   Serial.print(", millis: ");
  //   Serial.println(millis());
  //   idx++;
  // }

  for (int i = 0; i < beats_in_song; i++) {
    lightLEDs(leds, 50, CRGB::Blue, delay_time);
    // delay(delay_time);
    // delayMicroseconds(mils2micros(delay_time));
    lightLEDs(leds, 50, CRGB::Red, delay_time);
    // delay(delay_time);
    // delayMicroseconds(mils2micros(delay_time));
    Serial.print("idx: ");
    Serial.print(i);
    Serial.print(", millis: ");
    Serial.println(millis());
  }
  Serial.print("curr mils: ");
  Serial.println(millis());
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
void lightLEDs(CRGB* leds, int num_leds, CRGB color, int delay_time) {
  for (int i = 0; i < num_leds; i++) {
    leds[i] = color;
  }
  FastLED.show();
  delay(delay_time);
}