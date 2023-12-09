#include "alarm.h"
#include <FastLED.h>

// FSM variables
static int savedMillis, minuteCounter, lastReqMillis, lastReqSecondsSince1900, maxSnoozeTime, nextAlarmTime;
static bool downloadComplete;
char* songName;
char* newSongName;
int bpm;

// led related variables
#define NUM_LEDS 300 // # of LEDs in your strip
#define BRIGHTNESS  4 // 16 is a good value for this, was 64 in example code (max: 255)
CRGB leds[NUM_LEDS];


// pins
pin_size_t snoozeButton = 6; // TODO: modify these on official circuit
pin_size_t offButton = 7;
const int ledPin = 0; // TODO: add pins for LCD screen & other components

void setup() {
  Serial.begin(9600);
  while (!Serial)
    ;
  Serial.println("running setup...");
  Serial.println("in alarm.ino");

  /* initialize data pins */
  pinMode(snoozeButton, INPUT);
  pinMode(offButton, INPUT);

  /* initialize interrupts */
  attachInterrupt(snoozeButton, snoozeISR, RISING);
  attachInterrupt(offButton, alarmOffISR, RISING);

  /* initialize LEDs */
  FastLED.addLeds<WS2812B, ledPin, RGB>(leds, NUM_LEDS);  // GRB ordering is typical  
  setupLEDs(BRIGHTNESS);
  ledParty(leds, NUM_LEDS, CRGB::Brown, 60); // testing that it gets this far

  initializeSystem();  // TODO: might need to modify from lab 5
  savedMillis = millis();
  minuteCounter = 0;
  lastReqMillis = 0;
  lastReqSecondsSince1900 = 0;
  maxSnoozeTime = 600000; // 10 min, in millis
  downloadComplete = false;
  songName = "";  // TODO: are we starting with 1 pre downloaded? 
  newSongName = "";
  nextAlarmTime = INT32_MAX - 1;
  bpm = 60;
}

void loop() {
  static state CURRENT_STATE = sPROCESS_UPDATES;
  CURRENT_STATE = updateFSM(CURRENT_STATE, millis(), snoozeButtonPresses, stopButtonPresses);
  delay(10);
}

state updateFSM(state curState, long mils, int snoozePresses, int stopPresses) {
  state nextState;
  switch (curState) {
    case sPROCESS_UPDATES:  // TODO: will newSongName be updated fast enough? alternatively, should we have a boolean get set by one of the request functions, and use that as a guard (i.e. don't leave process updates state until bool is flipped?)
      if (newSongName != songName) {
        displayDownloadMessage();
        downloadComplete = false;
        nextState = sDOWNLOAD_SONG;
      } else {
        displayTime(lastReqSecondsSince1900 + (mils - lastReqMillis) / 1000);  // TODO: write this display function
        savedMillis = mils;
        minuteCounter = 0;
        nextState = sIDLE;
      }
      break;
    case sDOWNLOAD_SONG:
      if (downloadComplete) {
        displayTime(lastReqSecondsSince1900 + (mils - lastReqMillis) / 1000);
        songName = newSongName;
        minuteCounter = 0;
        nextState = sIDLE;
      } else {
        displayDownloadMessage();
        nextState = sDOWNLOAD_SONG;
      }
      break;
    case sIDLE:
      if (lastReqSecondsSince1900 + (mils - lastReqMillis) / 1000 >= nextAlarmTime) {
        displayAlarming();
        playSong(songName); // TODO: write play song
        // ledParty(); // TODO: modify to include proper params
        resetButtons();
        nextState = sALARMING;
      } else if (minuteCounter >= 5 && lastReqSecondsSince1900 + (mils - lastReqMillis) / 1000 + 600 < nextAlarmTime) {
        displayConnecting();
        maxSnoozeTime = requestSnoozeTime(); //TODO: write request function
        newSongName = requestSongName(); //TODO: write request function
        nextAlarmTime = requestAlarmTime(); //TODO: write request function (or could/should there just be 1 request function that pulls all data?) (val should be in sec since 1900)
        lastReqSecondsSince1900 = requestCurrTime(); //TODO: write request function
        nextState = sPROCESS_UPDATES;
      } else if (minuteCounter < 5 && mils - savedMillis >= 60000 && lastReqSecondsSince1900 + (mils - lastReqMillis) / 1000 < nextAlarmTime) {
        displayTime(lastReqSecondsSince1900 + (mils - lastReqMillis) / 1000);
        minuteCounter += 1;
        savedMillis = mils;
        nextState = sIDLE;
      } else {
        nextState = sIDLE;
      }
      break;
    case sALARMING:
      if (stopPresses > 0) {
        displayTime(lastReqSecondsSince1900 + (mils - lastReqMillis) / 1000);
        stopSound(); // TODO: write stop sound
        stopLEDs(); // TODO: write stop leds
        savedMillis = mils;
        minuteCounter = 0;
        nextState = sIDLE;
      } else if (stopPresses == 0 && snoozePresses > 0) {
        displaySnoozing(maxSnoozeTime);
        stopSound();
        stopLEDs();
        savedMillis = mils;
        nextState = sSNOOZING;
      } else {
        nextState = sALARMING;
      }
      break;
    case sSNOOZING:
      if (mils - savedMillis >= maxSnoozeTime) {
        displayAlarming();
        playSong(songName);
        // ledParty(); // TODO: fill in w proper args
        resetButtons();
        nextState = sALARMING;
      } else if (mils - savedMillis > 60000 && mils - savedMillis < maxSnoozeTime) {
        displaySnoozing(maxSnoozeTime - (mils - savedMillis));
        nextState = sSNOOZING;
      } else {
        nextState = sSNOOZING;
      }
      break;
    default:
      nextState = sIDLE;
      // Serial.println("ERROR: Shouldn't reach default case");
      break;
      return nextState;
  }
}
