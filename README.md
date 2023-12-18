# alarm-clock

## audioplayer.ino

- Dependencies:
	- AudioZero library
	- SD Library

- Pins on MKR1000:
	- 7 (**CAN CHANGE**): to CS on SD Card Reader
	- 8 (MOSI): to DI on SD Card Reader
	- 9 (SCK): to CLK on SD Card Reader
	- 10 (MISO): to D0 on SD Card Reader
	- A0: to +in on audio player


- SPECIAL NOTES:
	- You can use Examples>SD>CardInfo to check that the card is reading

- About the LEDs:
	- For the final submission, the only file that matters is LEDs.ino in the 
	  LEDs folder. We decided to include other files in a subfolder to show 
	  work that went into our project before we pivoted to power the LEDs on 
	  a second arduino due to them being blocking code. 
