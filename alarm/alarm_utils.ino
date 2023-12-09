/*
 * Here are functions that need to be filled in
 */ // TODO: should snoozeISR take in maxSnoozeTime if it's just incrementing button presses?
void snoozeISR() {
  Serial.println("snooze ISR entered!");
  snoozeButtonPresses++;
}

void alarmOffISR() {
  Serial.println("alarm off ISR entered!");
  stopButtonPresses++;
}

void playSong(char* songName) {
  Serial.println("running playSong()");
}

int requestSnoozeTime() {
  Serial.println("running requestSnoozeTime()");
}

// TODO: should this return a char array or a pointer to a char (char*)?
char* requestSongName() {
  Serial.println("running requestSongName()");
}

int requestAlarmTime() {
  Serial.println("running requestAlarmTime()");
}

int requestCurrTime() {
  Serial.println("running requestCurrTime()");
}

int requestBPM() {

}

void stopSound() {
  Serial.println("running stopSound()");
}

void stopLEDs() {
  Serial.println("running stopLEDs()");
}

void updateInputs() {
  // Serial.println("running updateInputs()");
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
  /* Get requests go here */
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
  // TODO: calculate curr time and print
  // NOTE: selena worked on this and is abandoning it with the hopes of sending via serial communication
  int secsInADay = 86400;
  int currTimeTodayInSecs = currTimeInSeconds % secsInADay; // time since 1900 % seconds in a day
}

void displaySnoozing(int snoozeTimeMS) {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("SNOOZED: ");
  lcd.print(snoozeTimeMS / 60000); // TODO: think about if this will result in incorrect rounding
  lcd.print(" min");
}

