#include "alarm.h"
#include <WiFi101.h>
#include <ArduinoHttpClient.h>
#include <SD.h>
#include <SPI.h>
#include <AudioZero.h>

const int chipSelect = 7;

// FSM variables
static int savedMillis, savedSnoozeMillis, minuteCounter, lastReqMillis, lastReqSecondsSince1970, maxSnoozeTime, nextAlarmTime;
static bool downloadComplete;
bool playingSong;
String songName;
String newSongName;
String weatherMessage;
response getResp;

const char serverAddress[] = "alarm-interface-d79607b746d4.herokuapp.com";
const int serverPort = 443;

WiFiSSLClient client;
HttpClient http(client, serverAddress, serverPort);

// pins
pin_size_t offButton = A1;
pin_size_t snoozeButton = A2;
const int ledPin = 0; 

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
  attachInterrupt(snoozeButton, snoozeISR, FALLING);
  attachInterrupt(offButton, alarmOffISR, FALLING);


  setupWiFi();

  initializeSystem();
  savedMillis = millis();
  savedSnoozeMillis = 0;
  playingSong = false;
  minuteCounter = 0;
  downloadComplete = false;
  songName = "";
  lastReqMillis = millis();
  while(!requestUpdate()) {
    delay(500);
  };
  saveWeather();
  maxSnoozeTime = getResp.snooze_in_ms;
  nextAlarmTime = getResp.alarm;
  newSongName = getResp.song_name;
  lastReqSecondsSince1970 = getResp.curr_time;
}

void loop() {
  static state CURRENT_STATE = sPROCESS_UPDATES;
  CURRENT_STATE = updateFSM(CURRENT_STATE, millis(), snoozeButtonPresses, stopButtonPresses);
  delay(10);
}

state updateFSM(state curState, long mils, int snoozePresses, int stopPresses) {
  state nextState;
  switch (curState) {
    case sPROCESS_UPDATES:
      if (newSongName != songName) {
        displayDownloadMessage();
        songName = newSongName;
        downloadComplete = false;
        downloadSong(newSongName);
        nextState = sDOWNLOAD_SONG;
        // FOLLOWING CODE WAS USED FOR DEMO, WHEN ALL SONGS WERE PRE-DOWNLOADED
        // displayTime(lastReqSecondsSince1970 + (mils - lastReqMillis) / 1000);
        // savedMillis = mils;
        // minuteCounter = 0;
        // nextState = sIDLE;
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
        savedMillis = mils;
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
        playSong(songName);
        resetButtons();
        nextState = sALARMING;
      } else if (minuteCounter >= 5 && lastReqSecondsSince1970 + (mils - lastReqMillis) / 1000 + 600 < nextAlarmTime) {
        displayConnecting();
        while(!requestUpdate());
        saveWeather();
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
      } else if (minuteCounter >= 5 && mils - savedMillis >= 60000 && lastReqSecondsSince1970 + (mils - lastReqMillis) / 1000 + 600 >= nextAlarmTime) {
        // edge case: has been in idle state for 5 min, but alarm is within 10 min
        // shouldn't request updates but needs to keep telling time
        displayTime(lastReqSecondsSince1970 + (mils - lastReqMillis) / 1000);
        savedMillis = mils;
        nextState = sIDLE;
      } else {
        nextState = sIDLE;
      }
      break;
    case sALARMING:
      if (stopPresses > 0) {
        displayTime(lastReqSecondsSince1970 + (mils - lastReqMillis) / 1000);
        savedMillis = mils;
        minuteCounter = 0;
        nextState = sIDLE;
      } else if (stopPresses == 0 && snoozePresses > 0) {
        displaySnoozing(maxSnoozeTime);
        savedMillis = mils;
        savedSnoozeMillis = mils;
        nextState = sSNOOZING;
      } else {
        nextState = sALARMING;
      }
      break;
    case sSNOOZING:
      if (mils - savedMillis >= maxSnoozeTime) {
        displayAlarming();
        resetButtons();
        playSong(songName);
        nextState = sALARMING;
      } else if (mils - savedSnoozeMillis > 60000 && mils - savedMillis < maxSnoozeTime) {
        displaySnoozing(maxSnoozeTime - (mils - savedMillis));
        savedSnoozeMillis = mils; 
        nextState = sSNOOZING;
      } else {
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
