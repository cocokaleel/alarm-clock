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
    while (true)
      ;
  }
  AudioZero.begin(2 * 44100);
  AudioZero.play(myFile);
}

void stopSound() {
  Serial.println("running stopSound()");
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
  lcd.begin(16, 2);
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
  lcd.setCursor(0, 0);
  lcd.print("WAKE UP");
  lcd.setCursor(0, 1);
  lcd.print(weatherMessage);
}

/*
 * Display download message
 */
void displayDownloadMessage() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("DOWNLOADING SONG");
}

/*
 * Display connecting
 */
void displayConnecting() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("CONNECTING...");
}

void displayTime(int currTimeInSeconds) {
  lcd.clear();
  lcd.setCursor(0, 0);
  int secsInADay = 86400;
  int currTimeTodayInSecs = currTimeInSeconds % secsInADay;  // time since 1900 % seconds in a day
  int currTimeTodayInMinutes = currTimeTodayInSecs / 60;
  int minuteHand = currTimeTodayInMinutes % 60;
  int currTimeTodayInHours = currTimeTodayInMinutes / 60;
  boolean isAfternoon = currTimeTodayInHours > 12;
  currTimeTodayInHours -= (isAfternoon ? 12 : 0);
  String time_string = String(currTimeTodayInHours) + ":" + minuteHand + " " + (isAfternoon ? "PM" : "AM");
  lcd.print(time_string);
  Serial.println("displaying time!");
}

void displaySnoozing(int snoozeTimeMS) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("SNOOZED: ");
  lcd.print(snoozeTimeMS / 60000);  // TODO: think about if this will result in incorrect rounding
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
      while ((http.connected() || http.available()) && (!http.endOfBodyReached()) && ((millis() - timeoutStart) < 30000)) {
        if (http.available()) {
          c = http.read();
          json += c;
          timeoutStart = millis();
        } else {
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
    } else {
      Serial.print("Getting response failed: ");
      Serial.println(err);
    }
  } else {
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
  String weather_report = "";
  int err = 0;
  err = http.get("/weather");
  if (err == 0) {
    err = http.responseStatusCode();
    if (err >= 0) {
      Serial.print("Got status code: ");
      Serial.println(err);
      unsigned long timeoutStart = millis();
      char c;
      while ((http.connected() || http.available()) && (!http.endOfBodyReached()) && ((millis() - timeoutStart) < 30000)) {
        if (http.available()) {
          c = http.read();
          weather_report += c;
          timeoutStart = millis();
        } else {
          delay(500);
        }
      }
      http.stop();
      return weather_report;
    } else {
      Serial.print("Getting response failed: ");
      Serial.println(err);
    }
  } else {
    Serial.print("Connect failed: ");
    Serial.println(err);
  }
  http.stop();
  return "";
}

String weatherDecoder(String code) {
  if (code == "00" || code == "01") {
    return "CLEAR";
  } else if (code == "03") {
    return "CLOUDY";
  } else if (code.charAt(0) == '0') {
    return "HAZE";
  } else if (code.charAt(0) == '3') {
    return "DUSTY";
  } else if (code.charAt(0) == '4') {
    return "FOG";
  } else if (code.charAt(0) == '5') {
    return "DRIZZLE";
  } else if (code.charAt(0) == '6') {
    return "RAIN";
  } else if (code.charAt(0) == '7') {
    return "SNOW";
  } else if (code.charAt(0) == '8') {
    return "SHOWERS";
  } else if (code == "95" || code == "96" || code == "97" || code == "98" || code == "99") {
    return "STORMS";
  } else {
    return "";
  }
}

void saveWeather() {
  String rawJSON = pullWeather();
  // if failed, try one more time
  rawJSON = (rawJSON.length() == 0 ? pullWeather() : rawJSON);
  String weatherJSON = rawJSON.substring(rawJSON.indexOf("\"code\":"));
  if (weatherJSON.length() == 0) {
    weatherMessage = "";
  } else {
    int first_break = weatherJSON.indexOf(',');
    int second_break = weatherJSON.indexOf(',', first_break + 1);
    String code = weatherJSON.substring(weatherJSON.indexOf(':') + 1, first_break);
    String high = weatherJSON.substring(weatherJSON.indexOf(':', first_break + 1) + 1, second_break);
    String low = weatherJSON.substring(weatherJSON.indexOf(':', second_break + 1) + 1, weatherJSON.length() - 1);
    String translated_code = weatherDecoder(code);
    weatherMessage = "H:" + high + " L:" + low + " " + translated_code;
    Serial.println("weather");
    Serial.println(weatherMessage);
  }
}

void downloadSong(String newSongName) {
  String testing = "";
  int err = 0;
  int readBytes = 0;
  err = http.get("/download-wav");
  if (err == 0) {
    err = http.responseStatusCode();
    if (err >= 0) {
      Serial.print("Got status code: ");
      Serial.println(err);
      unsigned long timeoutStart = millis();
      char c;
      File songFile = SD.open(newSongName, FILE_WRITE);
      while ((http.connected() || http.available()) && ((millis() - timeoutStart) < 30000)) {
        if (http.available()) {
          c = http.read();
          songFile.write(c);
          readBytes += 1;   
          testing += c;
          timeoutStart = millis();
        } else {
          delay(500);
        }
      }
    } else {
      Serial.print("Getting response failed: ");
      Serial.println(err);
    }
  } else {
    Serial.print("Connect failed: ");
    Serial.println(err);
  }
  http.stop();
  Serial.println("SONG DATA: ");
  Serial.println(testing);
  Serial.println("BYTES READ: ");
  Serial.println(readBytes);
  downloadComplete = true;
}
