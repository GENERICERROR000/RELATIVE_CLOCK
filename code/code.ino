/*
   Digital Clock with Relative Time

   When device is upside down, the time rotates
   180 degrees and shows a relative time.
   The relative time is set when the time is set.
   Relative time can be +/- hours:minutes.
   Time is set by using momentary push buttons.

   created 02/02/2020
   Noah Kernis
*/

// TODO: 
// - Need to access accelorometer to see if box is inverted
// - Logic for inverted code

// - BIND DOESNT WORK

// https://github.com/mathertel/OneButton/blob/master/examples/SimpleOneButton/SimpleOneButton.ino
#include <cstdio>
// time
#include <RTCZero.h>

RTCZero rtc;
int lastSecond = 0; // rtc.getSeconds()' previous value
String  timeStamp;

// buttons
// interrupt pins: 2, 3, 9, 10, 11, 13, 15, A5, A7
const int setBtn = 9;
const int upBtn = 3;
const int downBtn = 2;

#include "OneButton.h"
OneButton button(setBtn, true);

// config
boolean configMode = false;

// 0: hours, 1: minutes, 2: diffHours, 3: diffMinutes
int currentConfig = 0;

// relative time diff
// default is 12 hours 0 minutes ahead
int diffHours = 12;
int diffMinutes = 0;

// NOTE: -----> Setup <-----

void setup() {
  Serial.begin(9600);

  // init RTC
  rtc.begin();
  rtc.setTime(0, 0, 0);

  // set button
  pinMode(setBtn, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(setBtn), setPressed, FALLING);
  button.attachDoubleClick(setDoublePressed);

  // up button
  pinMode(upBtn, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(upBtn), upPressed, FALLING);

  // down button
  pinMode(downBtn, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(downBtn), downPressed, FALLING);
}

// NOTE: -----> Loop <-----

void loop() {
  // keep watching the push button
  button.tick();
  
  // check time has elapsed
  if (rtc.getSeconds() != lastSecond) {
    timeAction(0);

    lastSecond = rtc.getSeconds();
  }
}

// NOTE: -----> Action Handler <-----

void setPressed(){
  if (configMode == true) timeAction(2);
}

void setDoublePressed(){
  if (configMode == true) timeAction(1);
}

void upPressed(){
  if (configMode == true) timeAction(3);
}

void downPressed(){
  if (configMode == true) timeAction(4);
}

// NOTE: -----> Timestamp <-----

String createTimeStamp() {
  timeStamp = String(rtc.getHours()) + ':' + String(rtc.getMinutes()) + ':' + String(rtc.getSeconds());

  return timeStamp;
}

void displayTime(String time) {
  Serial.println(time);
}

//  NOTE: -----> Clock Config <-----

void timeAction(int action) {
  Serial.println(action);

  switch (action) {
    case 0: // TIME
      break;
    case 1: // DOUBLECLICK
      configMode = +configMode;
      break;
    case 2: // SET
      handleSet();
      break;
    case 3: // UP
      handleUp();
      break;
    case 4: // DOWN
      handleDown();
      break;
  }
  
  if (currentConfig == 0 || currentConfig == 1) {
    // TODO:
    // - Another check for box is inverted to determine if screen should be changed at all 
    displayTime(createTimeStamp());
  } else {
    // TODO:
    // - This should display the time diff for setting 
    // displayTime(createTimeStamp());
    displayTime(createTimeStamp());
  }
}

void handleSet() {
  if (currentConfig == 3) {
    currentConfig = 0;
  } else {
    currentConfig++;
  }
}

int whatToChange() {
  if (currentConfig == 0 || currentConfig == 2) {
    return rtc.getHours();
  } else {
    return rtc.getMinutes();
  }
}

void handleUp() {
  int time = whatToChange();

  switch (currentConfig) {
    case 0:
      rtc.setHours(time++);
      break;
    case 1:
      rtc.setMinutes(time++);
      break;
    case 2:
      break;
    case 3:
      break;
  }
}

void handleDown() {
  int time = whatToChange();

  switch (currentConfig) {
    case 0:
      rtc.setHours(time--);
      break;
    case 1:
      rtc.setMinutes(time--);
      break;
    case 2:
      // TODO: 
      // - need to check state of diff and then make decision (assign new value to diff var)
      // if (diffHours == 0) diffHours = 23;
      break;
    case 3:
      break;
  }
}
