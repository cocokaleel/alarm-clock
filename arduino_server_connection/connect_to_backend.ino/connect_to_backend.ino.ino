#include "helpers.h"
// globals

/* related to readiing in webpage */
char num_beats_buffer[10]; // initialize a character to hold the # of beats in a song

char beats_str_array[50 * 5]; // initialize w/ a large buffer
int beats_array[50];
int num_beats;


/* related to audio logic for running on LED 
 */
// int bpm; // an int of the bpm in a song
// int TOGGLE_TIME = 1000 / (bpm / 60); // TOGGLE_TIME represents a # in millis, saying every TOGGLE_TIME millis, toggle lights from on to off

/* logic for setting up actual LEDs */

void setup() {
  num_beats = 3;
  strcpy(beats_str_array, "466 496 525");
  // put your setup code here, to run once:

    //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial);
  Serial1.begin(9600); // may not need this

  setupWiFi();
  
  Serial.println("Something was received using HTTP!");

  // high level overview:
  // connect to webpage
  // send http get request (query the api)
  // read in what is on the webpage

  // initialize all relevant globals: num_beats, beats, bpm, toggle_time to values from server

  /* FIRST GET REQUEST */
  // note: this if statement is a stripped down version of connecToWebpage
  if (client.connect("172.18.142.84", 5001)) { // port is 80 for http, 443 for https
    // client.connect("rgb-led-app.heroku.com" 80);
    // get request example: client.println("GET /api/myFavArduino HTTP/1.1")
    Serial.println("connected to endpoint, sending get request");
    sendHTTPReqForNumBeats(); // gets the # of beats in the song
    readWebpage(num_beats_buffer, 10); // pass in array to write it to
    Serial.println(num_beats_buffer);
    Serial.println("finished reading from num_beats endpoint");
  } else {
    Serial.println("coudn't connect to num_beats endpoint");
  }

  /* SECOND GET REQUEST */
  /* after you get the # of beats, get the string of beat timestamps */
  // if (client.connect("http://172.18.142.84:5001/beats", 443)) {
  //   sendHTTPReqForBeats(); // gets the # of beats in the song
  //   readWebpage(beats_str_array, 100000); // pass in array to write it to
  // } else {
  //   Serial.println("coudn't connect to beats endpoint");
  // }

  Serial.println("reached the end of setup");


}

void loop() {
  // put your main code here, to run repeatedly:

  // parseBeatsStrArray(beats_str_array, beats_array, num_beats);
  
  delay(2000);

}

