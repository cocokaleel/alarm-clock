void snoozeISR() {
  Serial.println("snooze ISR entered!");
  snoozeButtonPresses++;
  AudioZero.end();
}

void alarmOffISR() {
  Serial.println("alarm off ISR entered!");
  stopButtonPresses++;
  AudioZero.end();
}

void playSong(String songName) {
  Serial.println("running playSong()");
  File myFile = SD.open("HOTTOG~1.wav");
  if (!myFile) {
    // if the file didn't open, print an error and stop
    Serial.println("error opening HOTTOG~1.wav");
    while (true);
  }
  AudioZero.begin(2*44100);
  AudioZero.play(myFile);
}

int requestBPM() {
//TODO
}

void stopSound() {
  Serial.println("running stopSound()");
}

void stopLEDs() {
  Serial.println("running stopLEDs()");
}

/*
 * FUNCTIONS THAT ARE FILLED IN BELOW THIS POINT
 /

/*
 * Initialize LCD based on Lab 5 schematic
 */
const int rs = 0, en = 1, d4 = 2, d5 = 3, d6 = 4, d7 = 5;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

/*
 * Initialize system: set up LCD
 */
void initializeSystem() {
  lcd.begin(16,2);

}

/*
 * Reset stop and snooze button counters to 0
 */
void resetButtons() {
  snoozeButtonPresses = 0;
  stopButtonPresses = 0;
}

/*
 * Display wake up message
 */
void displayAlarming() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("WAKE UP");
}

/*
 * Display download message
 */
void displayDownloadMessage() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("DOWNLOADING SONG");
}

/*
 * Display connecting
 */
void displayConnecting() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("CONNECTING...");
}

void displayTime(int currTimeInSeconds) {
  lcd.clear();
  lcd.setCursor(0,0);
  int secsInADay = 86400;
  int currTimeTodayInSecs = currTimeInSeconds % secsInADay; // time since 1900 % seconds in a day
  int currTimeTodayInMinutes = currTimeTodayInSecs / 60;
  int minuteHand = currTimeTodayInMinutes % 60;
  int currTimeTodayInHours = currTimeTodayInMinutes / 60;
  boolean isAfternoon = currTimeTodayInHours > 12;
  currTimeTodayInHours -= (isAfternoon ? 12 : 0);
  String time_string = String(currTimeTodayInHours)+":"+minuteHand+" "+(isAfternoon ? "PM" : "AM");
  lcd.print(time_string);
  Serial.println("displaying time!");
}

void displaySnoozing(int snoozeTimeMS) {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("SNOOZED: ");
  lcd.print(snoozeTimeMS / 60000); // TODO: think about if this will result in incorrect rounding
  lcd.print(" min");
}

void setupWiFi() {
  char ssid[] = "Brown-Guest";
  char pass[] = "";
  int status = WL_IDLE_STATUS;
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid);
    delay(2000);
  }
  Serial.println("Connected!");
}

bool requestUpdate() {
  String json = "";
  int err = 0;
  err = http.get("/all-settings");
  if (err == 0) {
    err = http.responseStatusCode();
    if (err >= 0) {
      Serial.print("Got status code: ");
      Serial.println(err);

      int bodyLen = http.contentLength();
      Serial.print("Content length is: ");
      Serial.println(bodyLen);
      Serial.println();
      Serial.println("Body:");
    
      unsigned long timeoutStart = millis();
      char c;
      while ( (http.connected() || http.available()) &&
             (!http.endOfBodyReached()) &&
             ((millis() - timeoutStart) < 30000) ) {
          if (http.available()) {
              c = http.read();
              json += c;
              timeoutStart = millis();
          }
          else {
              delay(500);
          }
      }
      Serial.println(json);
      getResp = parseJSON(json);
      http.stop();
      maxSnoozeTime = getResp.snooze_in_ms;
      newSongName = getResp.song_name;
      nextAlarmTime = getResp.alarm;
      lastReqSecondsSince1970 = getResp.curr_time;
      Serial.println(maxSnoozeTime);
      Serial.println(newSongName);
      Serial.println(nextAlarmTime);
      Serial.println(lastReqSecondsSince1970);
      return true;
    }
    else {    
      Serial.print("Getting response failed: ");
      Serial.println(err);
    }
  }
  else {
    Serial.print("Connect failed: ");
    Serial.println(err);
  }
  http.stop();
  return false;
}

response parseJSON(String json) {
  int first_break = json.indexOf(',');
  int second_break = json.indexOf(',', first_break + 1);
  int third_break = json.indexOf(',', second_break + 1);
  response res;
  res.alarm = json.substring(json.indexOf(':') + 1, first_break).toInt();
  res.snooze_in_ms = json.substring(json.indexOf(':', first_break + 1) + 1, second_break).toInt() * 60000;
  res.song_name = json.substring(json.indexOf(':', second_break + 1) + 2, third_break - 1);
  res.curr_time = json.substring(json.indexOf(':', third_break + 1) + 1, json.length() - 1).toInt();
  return res;
}

String pullWeather() {
  String json = "";
  int err = 0;
  err = http.get("/weather");
  if (err == 0) {
    err = http.responseStatusCode();
    if (err >= 0) {
      Serial.print("Got status code: ");
      Serial.println(err);
      unsigned long timeoutStart = millis();
      char c;
      while ( (http.connected() || http.available()) &&
             (!http.endOfBodyReached()) &&
             ((millis() - timeoutStart) < 30000)) {
          if (http.available()) {
              c = http.read();
              json += c;
              timeoutStart = millis();
          }
          else {
              delay(500);
          }
      }
      Serial.println(json);
      http.stop();
      return json;
    }
    else {    
      Serial.print("Getting response failed: ");
      Serial.println(err);
    }
  }
  else {
    Serial.print("Connect failed: ");
    Serial.println(err);
  }
  http.stop();
  return "";
}

void downloadSong(newSongName) {
  int err = 0;
  err = http.get("/download-wav");
  if (err == 0) {
    err = http.responseStatusCode();
    if (err >= 0) {
      Serial.print("Got status code: ");
      Serial.println(err);
      unsigned long timeoutStart = millis();
      char c;
      while ( (http.connected() || http.available()) &&
             (!http.endOfBodyReached()) &&
             ((millis() - timeoutStart) < 30000) ) {
          if (http.available()) {
              c = http.read();
              
              timeoutStart = millis();
          }
          else {
              delay(500);
          }
      }
    }
    else {    
      Serial.print("Getting response failed: ");
      Serial.println(err);
    }
  }
  else {
    Serial.print("Connect failed: ");
    Serial.println(err);
  }
  http.stop();
  downloadComplete = true;
}
