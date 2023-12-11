#include "alarm.h"
#include <WiFi101.h>
#include <ArduinoHttpClient.h>
#include <SD.h>
#include <SPI.h>
#include <AudioZero.h>

const int chipSelect = 7;

// FSM variables
static int savedMillis, minuteCounter, lastReqMillis, lastReqSecondsSince1970, maxSnoozeTime, nextAlarmTime;
static bool downloadComplete;
String songName;
String newSongName;
response getResp;
int bpm;

const char serverAddress[] = "alarm-interface-d79607b746d4.herokuapp.com";
const int serverPort = 443;

WiFiSSLClient client;
HttpClient http(client, serverAddress, serverPort);

// led related variables
#define NUM_LEDS 300 // # of LEDs in your strip
#define BRIGHTNESS 2 // 16 is a good value for this, was 64 in example code (max: 255)
CRGB leds[NUM_LEDS];
CRGB color1 = CRGB::Green;
CRGB color2 = CRGB::Black;

// pins
pin_size_t snoozeButton = A1; // TODO: modify these on official circuit
pin_size_t offButton = A2;
const int ledPin = 0; // TODO: add pins for LCD screen & other components

void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.print("Initializing SD card...");
  if (!SD.begin(chipSelect)) {
    Serial.println(" failed!");
    while(true);
  }
  Serial.println(" done.");

  /* initialize data pins */
  pinMode(snoozeButton, INPUT);
  pinMode(offButton, INPUT);

  /* initialize interrupts */
  attachInterrupt(snoozeButton, snoozeISR, RISING);
  attachInterrupt(offButton, alarmOffISR, RISING);

  /* initialize LEDs */
  FastLED.addLeds<WS2812B, ledPin, RGB>(leds, NUM_LEDS);  // GRB ordering is typical  
  setupLEDs(BRIGHTNESS);
  setupWiFi();

  initializeSystem();
  savedMillis = millis();
  minuteCounter = 0;
  downloadComplete = false;
  songName = "";
  bpm = 30;
  lastReqMillis = millis();
  while(!request_update()) {
    delay(500);
  };
  maxSnoozeTime = getResp.snooze_in_ms;
  nextAlarmTime = getResp.alarm;
  newSongName = getResp.song_name;
  lastReqSecondsSince1970 = getResp.curr_time;
  Serial.println("made it");
  Serial.println(maxSnoozeTime);
}

void loop() {
  static state CURRENT_STATE = sPROCESS_UPDATES;
  Serial.println("STATE");
  Serial.println(CURRENT_STATE);
  CURRENT_STATE = updateFSM(CURRENT_STATE, millis(), snoozeButtonPresses, stopButtonPresses);
  
  delay(10);
}

state updateFSM(state curState, long mils, int snoozePresses, int stopPresses) {
  state nextState;
  switch (curState) {
    case sPROCESS_UPDATES:
      if (newSongName != songName) {
        displayDownloadMessage();
        downloadComplete = false;
        nextState = sDOWNLOAD_SONG;
      } else {
        displayTime(lastReqSecondsSince1970 + (mils - lastReqMillis) / 1000);
        savedMillis = mils;
        minuteCounter = 0;
        nextState = sIDLE;
      }
      break;
    case sDOWNLOAD_SONG:
      if (downloadComplete) {
        displayTime(lastReqSecondsSince1970 + (mils - lastReqMillis) / 1000);
        songName = newSongName;
        minuteCounter = 0;
        nextState = sIDLE;
      } else {
        displayDownloadMessage();
        nextState = sDOWNLOAD_SONG;
      }
      break;
    case sIDLE:
      if (lastReqSecondsSince1970 + (mils - lastReqMillis) / 1000 >= nextAlarmTime) {
        displayAlarming();
        playSong(songName); // TODO: write play song
        ledParty(leds, NUM_LEDS, color1, color2, bpm);
        resetButtons();
        nextState = sALARMING;
      } else if (minuteCounter >= 5 && lastReqSecondsSince1970 + (mils - lastReqMillis) / 1000 + 600 < nextAlarmTime) {
        displayConnecting();
        while(!request_update());
        maxSnoozeTime = getResp.snooze_in_ms;
        nextAlarmTime = getResp.alarm;
        newSongName = getResp.song_name;
        lastReqSecondsSince1970 = getResp.curr_time;
        nextState = sPROCESS_UPDATES;
      } else if (minuteCounter < 5 && mils - savedMillis >= 60000 && lastReqSecondsSince1970 + (mils - lastReqMillis) / 1000 < nextAlarmTime) {
        displayTime(lastReqSecondsSince1970 + (mils - lastReqMillis) / 1000);
        minuteCounter += 1;
        savedMillis = mils;
        nextState = sIDLE;
      } else {
        nextState = sIDLE;
      }
      break;
    case sALARMING:
      if (stopPresses > 0) {
        displayTime(lastReqSecondsSince1970 + (mils - lastReqMillis) / 1000);
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
        ledParty(leds, NUM_LEDS, color1, color2, bpm); 
        resetButtons();
        nextState = sALARMING;
      } else if (mils - savedMillis > 60000 && mils - savedMillis < maxSnoozeTime) {
        displaySnoozing(maxSnoozeTime - (mils - savedMillis));
        nextState = sSNOOZING;
      } else { // if it's less than 1 min or less than maxSnoozeTime, continue snoozing
        nextState = sSNOOZING;
      }
      break;
    default:
      nextState = sIDLE;
      Serial.println("ERROR: Shouldn't reach default case");
      break;
  }
  return nextState;
}
