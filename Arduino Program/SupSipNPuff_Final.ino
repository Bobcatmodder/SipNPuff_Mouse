/*
MIT License

Copyright (c) 2018 Jacob Field

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

--The source code, design files, and links to documentation for this project can
--be found at https://github.com/Bobcatmodder/SipNPuff_Mouse/.
*/

#include <Mouse.h>

//Pressure sensor sampling values
volatile byte sampleCounter = 0;
byte sampleLength = 12; //desired sample number minus one because computers are zero indexed
volatile int samples[13];
unsigned long sampleRoutineTimer = 0;
byte samplePeriod = 8;

//Mouse button state trackers
bool isLeftPressed = false;
bool isRightPressed = false;

//Cursor velocity variables (to do with precision and speed)
int wavelength = 15; // Distance/Period (Speed of cursor in pixels/iteration)
int period = 35; // Time/period (MS per iteration)
unsigned long cursorFrequencyTimer = 0; // Variable to time when to update the mouse

//Joystick threshold and analog value storing arrays
int joyThresh[] = {5, 5}; //Deadzone threshholds for X and Y axis of joystick
int reading[] = {0, 0}; //Array for storing the values of the readings from the joystick axis


void setup() { //Setup code, runs once at startup

  Mouse.begin(); //Take control of the mouse

  //Initialize the timers (these are used so we don't have to use a delay() function inside the loop)
  sampleRoutineTimer = millis() + samplePeriod;
  cursorFrequencyTimer = millis() + period;

}



void loop() { //Main loop, runs over and over as fast as possible

  //Code to run the sampling routine roughly every samplePeriod MS
  if (millis() >= sampleRoutineTimer) { //Otherwise if it's equal or less
    samplePressure();//Go run the function to sample the analog value from the pressure sensor
    sampleRoutineTimer = millis() + samplePeriod;//Reset the timer to 25 MS ahead of current millis() value
  }

  //If the sample buffer length has reached the desired number of samples, then X
  if (sampleCounter >= sampleLength + 1) {
    inputMouse(averagePressureValue());
    sampleCounter = 0; // Reset the sample buffer counter to zero
  }

  //Code to run the joystick analog value reading/conversion and mouse updating code every period MS
  if (millis() >= cursorFrequencyTimer) { //Otherwise if it's equal or less
    readAxis(1, 2);
    Mouse.move(reading[0], -reading[1], 0);
    cursorFrequencyTimer = millis() + period;//Reset the timer to 25 MS ahead of current millis() value
  }

}



//----------------Custom Functions----------------

//Sampling function to run every time the timer calls it
void samplePressure() {
  samples[sampleCounter] = analogRead(A0);
  sampleCounter += 1;
}


//Function to return the average value of the sample array
int averagePressureValue(){
  unsigned int averageReading = 0;
    for (byte x = 0; x <= sampleLength; x++) {
      averageReading += samples[x];
    }
    averageReading = averageReading / sampleLength + 1;
    return averageReading;
}


//Function that takes the average sample array pressure value, then sends a mouse command based off that pressure value
void inputMouse(int pressValue){
    if (pressValue < 400) { //Hard sip
      if (!isRightPressed) {
        Mouse.press(MOUSE_RIGHT);
        isRightPressed = true;
      }
    } else if (pressValue > 415 && pressValue < 535) { //Soft sip
      Mouse.move(0, 0, -1);
    } else if (pressValue > 550 && pressValue < 600) {
      if (isLeftPressed) {
        Mouse.release(MOUSE_LEFT);
        isLeftPressed = false;
      }
      if (isRightPressed) {
        Mouse.release(MOUSE_RIGHT);
        isRightPressed = false;
      }
    } else if (pressValue > 615 && pressValue < 675) { //Soft puff
      Mouse.move(0, 0, 1);
    } else if (pressValue > 690) { //Hard puff
      if (!isLeftPressed) {
        Mouse.press(MOUSE_LEFT);
        isLeftPressed = true;
      }
    }
}


//Function to read the analog values from the joystick, remove noise in the deadzone, then convert them to useable +/- values
void readAxis(int pinOne, int pinTwo){
  //Read initial values
  reading[0] = analogRead(pinOne) - 22;
  reading[1] = analogRead(pinTwo);
  //Subtract 511 to get a -511 to 511 value from the 0-1023 value
  for(byte x = 0; x <= 1; x++){
    reading[x] -= 511;
  }
  //Check to see if the values are below the threshold, if so, erase them.
  for(byte x = 0; x <= 1; x++){
    if(abs(reading[x]) < joyThresh[x]){
      reading[x] = 0;
    } else {
      if(reading[x] < 0){
        reading[x] += joyThresh[x];
      } else {
        reading[x] -= joyThresh[x];  
      }
    }
  }
  //Map values to lower numbers, and negative/positive values
  for(byte x = 0; x <= 1; x++){
    reading[x] = reading[x] / (1023 / (wavelength *2)); //map(reading[x], 0, 1023, -cursorSpeed, cursorSpeed); <- This was the old one, I didn't want to rely on that though
  }
}
