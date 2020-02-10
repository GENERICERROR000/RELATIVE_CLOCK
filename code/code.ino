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

// https://github.com/mathertel/OneButton/blob/master/examples/SimpleOneButton/SimpleOneButton.ino

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
string currentConfig = 0;

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
  attachInterrupt(digitalPinToInterrupt(setBtn), std::bind(handleAction, "SET"), FALLING);
  button.attachDoubleClick(std::bind(handleAction, "DOUBLECLICK"))

  // up button
  pinMode(upBtn, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(upBtn), std::bind(handleAction, "UP"), FALLING);

  // down button
  pinMode(downBtn, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(downBtn), std::bind(handleAction, "DOWN"), FALLING);
}

// NOTE: -----> Loop <-----

void loop() {
  // keep watching the push button
  button.tick();
  
  // check time has elapsed
  if (rtc.getSeconds() != lastSecond) {
    timeAction("TIME");

    lastSecond = rtc.getSeconds();
  }
}

// NOTE: -----> Button Handler <-----

void handleAction(string action){
  if (configMode == true) timeAction(action);
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

void timeAction(string action) {
  Serial.println(action);

  switch (action)
    case "TIME":
      break;
    case "DOUBLECLICK":
      configMode = +configMode;
      break;
    case "SET":
      handleSet();
      break;
    case "UP":
      handleUp();
      break;
    case "DOWN":
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
  }
}

void handleSet() {
  if (currentConfig == 3) {
    currentConfig = 0;
  } else {
    currentConfig++
  }
}

void handleUp() {
  int time = whatToChange();

  switch (currentConfig)
    case 0:
      rtc.setHours(time++);
    case 1:
      rtc.setMinutes(time++);
    case 2:
    // ...
    case 3:
    // ...
}

void handleDown() {
  int time = whatToChange();

  switch (currentConfig)
    case 0:
      rtc.setHours(time--);
    case 1:
      rtc.setMinutes(time--);
    case 2:
      // TODO: 
      // - need to check state of diff and then make decision (assign new value to diff var)
      // if (diffHours == 0) diffHours = 23;
    case 3:
    // ...
  }
}

int whatToChange() {
  if (currentConfig == 0 || currentConfig == 2) {
    return rtc.getHours();
  } else {
    return rtc.getMinutes();
  }
}
