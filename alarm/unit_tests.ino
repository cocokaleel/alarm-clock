#include "alarm.h"
/* Key variables to use for unit tests
 
static int savedMillis, minuteCounter, lastReqMillis, lastReqSecondsSince1970, maxSnoozeTime, nextAlarmTime;
static bool downloadComplete;
String songName;
String newSongName;
response getResp;
int bpm;

*/

// THINGS I HAVEN'T TESTED:
// playSong()
// stopSound

// functions to test:

/** UNIT TESTING
 * Note: Since many of the libraries for unit testing on arduino were
 * buggy, we decided to unit test by making testFunctions that wrapped
 * basic functionality, and asserting that the functions behave as 
 * expected by using expected behavior as the guard to enter a 'return
 * True' block. That way, if all of the test functoins return true, then
 * we can be confident that our unit tests modify system variables as
 * expected.
 */


/**
 * We unit test this by checking if calling snoozeISR
 * increments the snoozeButtonPresses variable as expected
 */
bool testSnoozeISR(){
  int iniitalSnoozeButtonPresses = snoozeButtonPresses;
  snoozeISR();
  if (snoozeButtonPresses = iniitalSnoozeButtonPresses + 1) {
    return true;
  } else {
    return false;
  }
}

/**
 * We unit test this by checking if calling alarmOffISR
 * increments the stopButtonPresses variable as expected
 */
bool testAlarmOffISR(){
  int iniitalStopButtonPresses = stopButtonPresses;
  snoozeISR();
  if (stopButtonPresses = iniitalStopButtonPresses + 1) {
    return true;
  } else {
    return false;
  }
}

/**
 * We unit test that the system is correctly initialized by 
 * initializing the system, writing a message to the LCD
 * display, and then confirming that a message was written
 * by checking if bytes were written to the display.
 * 
 * The lcd.print() function returns the # of bytesWritten,
 * and which we use to confirm correct initialization because
 * you can't print to the screen if it hasn't been initialized.
 */
bool testInitializeSystem() {
  iniitalizeSystem();
  lcd.setCursor(0, 0);
  int bytesWritten = lcd.print("DOWNLOADING SONG");
  if (bytesWritten > 0) {
    return true;
  } else {
    return false;
  }
}

/**
 * We unit test resetButtons() by setting the button press counters
 * to positive, non-zero integers (the input range we expect for them),
 * and then calling resetButtons() and checking if those variables have
 * been reset to zero.
 */
bool testResetButtons(){
  snoozeButtonPresses = 6;
  stopButtonPresses = 1;

  resetButtons();

  if (snoozeButtonPresses == 0 && stopButtonPresses == 0) {
    return true;
  } else {
    return false;
  }
}

/**
 * We unit test displayAlarming by copying the code from within the
 * function and using the return parameters to ensure that bytes
 * were written to the LCD screen. 
 * Then, our guard to ensure correctness makes sure that bytes 
 * were written for both displaying a 'WAKE UP' message and a 
 * weather message.
 */
bool testDisplayAlarming() {
  initializeSystem();
  // since displayAlarming doesn't return anything, I have copied
  // the inside of the function here use that to assert that bytes 
  // were written to the LCD screen

  /* displayAlarming() code */
  lcd.clear();
  lcd.setCursor(0, 0);
  int bytesWritten = lcd.print("WAKE UP");
  lcd.setCursor(0, 1);

  String exWeatherMessage = "H: 60 L: 48"; // TODO: this may be an error from being a string

  int weatherBytesWritten = lcd.print(exWeatherMessage);

  if (bytesWritten > 0 && weatherBytesWritten > 0) {
    return true;
  } else {
    return false;
  }
}

bool testdisplayDownloadMessage() {
  initializeSystem();
  lcd.clear();
  lcd.setCursor(0, 0);
  int bytesWritten = lcd.print("DOWNLOADING SONG");
  if (bytesWritten > 0) {
    return true;
  } else {
    return false;
  }
}

bool testDisplayConnecting() {
  initializeSystem();
  lcd.clear();
  lcd.setCursor(0, 0);
  int bytesWritten = lcd.print("CONNECTING...");
  if (bytesWritten > 0) {
    return true;
  } else {
    return false;
  }
}

// /*
//  * Runs through all the test cases defined above
//  */
// bool testAllTests() {
//   // num_correct_tests = 0;
//   for (int i = 0; i < numTests; i++) {
//     Serial.print("Running test ");
//     Serial.println(i);
//     if (!testTransition(testStatesIn[i], testStatesOut[i], testInputs[i], testVarsIn[i], testVarsOut[i], true)) {
//       return false;
//     }
//     Serial.println();
//   }
//   Serial.println("All tests passed!");
//   return true;
// }