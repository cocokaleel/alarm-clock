// /* Key variables to use for unit tests
 
// static int savedMillis, minuteCounter, lastReqMillis, lastReqSecondsSince1970, maxSnoozeTime, nextAlarmTime;
// static bool downloadComplete;
// String songName;
// String newSongName;
// response getResp;
// int bpm;

// */

// // THINGS I HAVEN'T TESTED:
// // playSong()

// // functions to test:


// bool testSnoozeISR(){
//   int iniitalSnoozeButtonPresses = snoozeButtonPresses;
//   snoozeISR();
//   if (snoozeButtonPresses = iniitalSnoozeButtonPresses + 1) {
//     return true;
//   } else {
//     return false;
//   }
// }

// bool testAlarmOffISR(){
//   int iniitalStopButtonPresses = stopButtonPresses;
//   snoozeISR();
//   if (stopButtonPresses = iniitalStopButtonPresses + 1) {
//     return true;
//   } else {
//     return false;
//   }
// }


// // /*
// //  * Runs through all the test cases defined above
// //  */
// // bool testAllTests() {
// //   // num_correct_tests = 0;
// //   for (int i = 0; i < numTests; i++) {
// //     Serial.print("Running test ");
// //     Serial.println(i);
// //     if (!testTransition(testStatesIn[i], testStatesOut[i], testInputs[i], testVarsIn[i], testVarsOut[i], true)) {
// //       return false;
// //     }
// //     Serial.println();
// //   }
// //   Serial.println("All tests passed!");
// //   return true;
// // }