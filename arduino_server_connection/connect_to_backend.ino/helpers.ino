char httpGETbuf[200]; // to form HTTP GET request


/*
 * Connect to WiFi network, get NTP time, and connect to random.org
 */
void setupWiFi() {
  char ssid[] = "Brown-Guest";  // network SSID (name)
  char pass[] = ""; // for networks that require a password
  int status = WL_IDLE_STATUS;  // the WiFi radio's status

  // attempt to connect to WiFi network:
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid); // WiFi.begin(ssid, pass) for password
    delay(5000);
  }
  Serial.println("Connected to wifi!");

  // if (connectToWebpage()) {
  //   Serial.println("fetched webpage");
  // } else {
  //   Serial.println("ERROR: failed to fetch webpage");
  //   Serial.println("Are SSL certificates installed?");
  //   while(true); // 
  // }
  Serial.println();
}

//                                  beats_str_array       beats_array       num_beats
void parseBeatsStrArray(char* beats_str_array, int* beats_array, int& beats_array_size) {
  // goes through beats_str_array, splitting chars on string to get ints
  // a str_array of " 3 16 28 287" gets parsed into beats_array = [3, 16, 28, 287]
  Serial.print("beats str array is ");
  Serial.println(beats_str_array);

  int outputSize = 0; // Initialize the size of the output array

  char* token = strtok(beats_str_array, " "); // Split the string using space as the delimiter

  while (token != NULL) {
    beats_array[outputSize++] = atoi(token); // Convert the token to an integer and add it to the array
    token = strtok(NULL, " "); // Move to the next token
  }
  
  printIntArray(beats_array, beats_array_size);

  Serial.println("finished running parseBeatsArray()");
  Serial.println();

}

void printIntArray(int* intArray, int& arraySize){
  Serial.println("printing int array");
  Serial.print("[");
  for (int i=0; i < arraySize; i++) {
    Serial.print(intArray[i]);
    if (i < arraySize -1) {
      Serial.print(", ");
    }
  }
  Serial.println("]");

}



void sendHTTPReqForNumBeats() {
  // GET /num_beats HTTP/1.1
  // sprintf(httpGETbuf, "GET /num_beats HTTP/1.1");
  // sprintf(httpGETbuf, "GET 172.18.142.84:5001/num_beats");
  sprintf(httpGETbuf, "GET /num_beats HTTP/1.1");
  client.println(httpGETbuf);
  client.println("Getting number of beats");
  client.println();
}

void sendHTTPReqForBeats() {
  sprintf(httpGETbuf, "GET /beats");
  client.println(httpGETbuf);
  client.println("Getting beat timestamp data");
  client.println();
}

// maxChars should be the size of charArray
bool readWebpage(char* charArray, int maxChars) {
  int len = client.available();
  if (len == 0){
    return false;
  }
  
  int idx = 0;
  while (client.available()) {
  // read in all the chars on the page
    char val = (char) client.read();
    charArray[idx] = (char) client.read(); // now this has read in all the words on the page
    idx++;
  }

  // // disable interrupts
  // // check that sBuf will not fill before adding to it
  // if (!(sBufStart == (sBufEnd + 1) % maxChars)) { 
  //   Serial.print("writing ");
  //   Serial.print((char) client.read());
  //   Serial.println(" to buffer");
  //   noInterrupts(); // disable interrupts
  //   sBuf[sBufEnd] = val;  // add to buffer
  //   interrupts(); // re-enable interrupts
  //   sBufEnd = (sBufEnd + 1) % maxChars; // increment end
  // } else { // if the array is full and is about to overflow: 
  //   Serial.print("ERROR: line 38 in readWebpage: ");
  //   Serial.println("character buffer ran out of space when reading in values");
  // }

  Serial.println();
  return true;
  }