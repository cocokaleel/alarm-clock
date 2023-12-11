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
String weatherMessage;
response getResp;

const char serverAddress[] = "alarm-interface-d79607b746d4.herokuapp.com";
const int serverPort = 443;

WiFiSSLClient client;
HttpClient http(client, serverAddress, serverPort);

// pins
pin_size_t snoozeButton = A1; // TODO: modify these on official circuit
pin_size_t offButton = A2;

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
  //attachInterrupt(snoozeButton, snoozeISR, RISING);
  //attachInterrupt(offButton, alarmOffISR, RISING);
  setupWiFi();

  initializeSystem();
  savedMillis = millis();
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
  Serial.print("in setup, new song name is: ");
  Serial.print(newSongName);
  Serial.print(" and songName is ");
  Serial.println(songName);
}

void loop() {
  Serial.print("running loop, seconds is ");
  int seconds = millis() / 1000;
  Serial.println(seconds);
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
        downloadSong(newSongName);
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
      } else if (minuteCounter >= 5 && lastReqSecondsSince1970 + (mils - lastReqMillis) / 1000 + 600 >= nextAlarmTime) {
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
        stopSound(); // TODO: write stop sound
        savedMillis = mils;
        minuteCounter = 0;
        nextState = sIDLE;
      } else if (stopPresses == 0 && snoozePresses > 0) {
        displaySnoozing(maxSnoozeTime);
        stopSound();
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
