
typedef struct {
  static bool downloadComplete;
  int snoozePresses;
  int stopButtonPresses;
} state_inputs;

typedef struct {
static int savedMillis, minuteCounter, lastReqMillis, lastReqSecondsSince1900, maxSnoozeTime, nextAlarmTime;
char* songName;
char* newSongName;
int bpm;
} state_vars;


bool testTransition(state startState,
                     state endState,
                     state_inputs testStateInputs, 
                     state_vars startStateVars,
                     state_vars endStateVars,
                     bool verbos) {

  savedMillis = startStateVars.savedMillis;
  minuteCounter = startStateVars.minuteCounter;
  lastReqMillis = startStateVars.lastReqMillis;
  lastReqSecondsSince1900 = startStateVars.lastReqSecondsSince1900;
  maxSnoozeTime = startStateVars.maxSnoozeTime;
  nextAlarmTime = startStateVars.nextAlarmTime;
  songName = startStateVars.songName;
  newSongName = startStateVars.newSongName;
  bpm = startStateVars.bpm;

  state resultState = updateFSM(startState, testStateInputs.mils, testStateInputs.numButtons, testStateInputs.lastButton);

  bool passedTest = (endState == resultState and
                      savedMillis == endStateVars.savedMillis and
                      minuteCounter == endStateVars.minuteCounter and
                      lastReqMillis == endStateVars.lastReqMillis and
                      lastReqSecondsSince1900 == endStateVars.lastReqSecondsSince1900 and
                      maxSnoozeTime == endStateVars.maxSnoozeTime and
                      nextAlarmTime == endStateVars.nextAlarmTime and
                      songName == endStateVars.songName and
                      newSongName == endStateVars.newSongName and
                      bpm == endStateVars.bpm);
  if (! verbos) {
    return passedTest;
  } else if (passedTest) {
    char sToPrint[200];
    sprintf(sToPrint, "Test from %s to %s PASSED", s2str(startState), s2str(endState));
    Serial.println(sToPrint);
    return true;
  } else {
    char sToPrint[200];
    Serial.println(s2str(startState));
    sprintf(sToPrint, "Test from %s to %s FAILED", s2str(startState), s2str(endState));
    Serial.println(sToPrint);
    sprintf(sToPrint, "End state expected: %s | actual: %s", s2str(endState), s2str(resultState));
    Serial.println(sToPrint);
    sprintf(sToPrint, "Inputs: mils %ld | numButtons %d | lastButton %s", testStateInputs.mils, testStateInputs.numButtons, o2str(testStateInputs.lastButton));
    Serial.println(sToPrint);
    sprintf(sToPrint, "          %2s | %2s | %5s | %3s | %3s | %5s | %9s | %11s | %9s", "x", "y", "o", "lxb", "uxb", "level", "timeStep", "savedClock", "countdown");
    Serial.println(sToPrint);
    sprintf(sToPrint, "expected: %2d | %2d | %5s | %3d | %3d | %5d | %9d | %11d | %9d", endStateVars.x, endStateVars.y, o2str(endStateVars.o), endStateVars.lxb, endStateVars.uxb, endStateVars.level, endStateVars.timeStep, endStateVars.savedClock, endStateVars.countdown);
    Serial.println(sToPrint);
    sprintf(sToPrint, "actual:   %2d | %2d | %5s | %3d | %3d | %5d | %9d | %11d | %9d", x, y, o2str(o), lxb, uxb, level, timeStep, savedClock, countdown);
    Serial.println(sToPrint);
    return false;
  }
}


/*
 * Runs through all the test cases defined above
 */
bool testAllTests() {
  // num_correct_tests = 0;
  for (int i = 0; i < numTests; i++) {
    Serial.print("Running test ");
    Serial.println(i);
    if (!testTransition(testStatesIn[i], testStatesOut[i], testInputs[i], testVarsIn[i], testVarsOut[i], true)) {
      return false;
    }
    Serial.println();
  }
  Serial.println("All tests passed!");
  return true;
}