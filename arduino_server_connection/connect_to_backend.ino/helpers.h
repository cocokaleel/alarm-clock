#include <SPI.h>
#include <WiFi101.h>
#include <WiFiUdp.h>

/* HTTP helper function definitions */
void setupWiFi();
bool connectToWebpage();
bool readWebpage();
void sendHTTPReqForNumBeats();
void sendHTTPReqForBeats();
void parseBeatsStrArray();

/* low level helpers */
void printIntArray();


// WiFi client
WiFiSSLClient client;


/* send and receive buffers for UART communication */
const int rsBufSize = 80;
byte sBuf[rsBufSize];
int sBufStart = 0;
int sBufEnd = 0;
byte rBuf[rsBufSize];
int rBufStart = 0;
int rBufEnd = 0;