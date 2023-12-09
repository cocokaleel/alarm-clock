#include <LiquidCrystal.h>
#include <FastLED.h>


typedef enum {
  sPROCESS_UPDATES = 1,
  sDOWNLOAD_SONG = 2,
  sIDLE = 3,
  sALARMING = 4,
  sSNOOZING = 5,
} state;

/*
 * Input variables
 */
 int snoozeButtonPresses;
 int stopButtonPresses;

/*
 * Setup functions
 */
void initializeSystem();

/*
 * Read in buttons and update corresponding variables
 */
void updateInputs();

/*
 * Helper function definitions
 */
void resetButtons();
void displayAlarming();
void displayDownloadMessage();
void displayConnecting();
void displayTime(int currTimeInSeconds);
void displaySnoozing(int snoozeTimeMS);

state updateFSM(state curState, long mils, int snoozePresses, int stopPresses);

/* 
 * ISRs
 */
void snoozeISR(); // Note: modified to remove maxSnoozeTime
void alarmOffISR();

/*
 * LEDS
 */
#include <stdio.h>
#include <stdint.h>
#include <FastLED.h>

/*
 * Helper functions from ledHelpers.ino 
 */
void setupLEDs(int brightness);
uint32_t mils2micros(uint32_t milliseconds);
void lightLEDs(CRGB* leds, int num_leds, CRGB color);
