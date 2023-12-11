#include <LiquidCrystal.h>
#include <FastLED.h>


typedef enum {
  sPROCESS_UPDATES = 1,
  sDOWNLOAD_SONG = 2,
  sIDLE = 3,
  sALARMING = 4,
  sSNOOZING = 5,
} state;

typedef struct {
  int alarm;
  int snooze_in_ms;
  String song_name;
  int curr_time;
} response;

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
void downloadSong(String newSongName);

state updateFSM(state curState, long mils, int snoozePresses, int stopPresses);

/* 
 * ISRs
 */
void snoozeISR(); // Note: modified to remove maxSnoozeTime
void alarmOffISR();