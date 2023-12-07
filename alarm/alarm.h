#include <LiquidCrystal.h>

typedef enum {
  sPROCESS_UPDATES = 1,
  sDOWNLOAD_SONG = 2,
  sIDLE = 3,
  sALARMING = 4,
  sSNOOZING = 5,
} state;

/*
 * Input variables
 */
 int snoozeButtonPresses;
 int stopButtonPresses;

/*
 * Setup functions
 */
void initializeSystem();

/*
 * Read in buttons and update corresponding variables
 */
void updateInputs();

/*
 * Helper function definitions
 */
void resetButtons();
void displayAlarming();
void displayDownloadMessage();
void displayConnecting();
void displayTime(int currTimeInSeconds);
void displaySnoozing(int snoozeTimeMS);

state updateFSM(state curState, long mils, int snoozePresses, int stopPresses);
