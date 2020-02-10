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

// display
//#include "SPI.h"
//#include "TFT_22_ILI9225.h"
//
//#define USE_ARDUINO_INTERRUPTS true
//#define TFT_CLK 13  // SCK
//#define TFT_SDI 11  // MOSI
//#define TFT_CS  10  // SS
//#define TFT_RS  9
//#define TFT_RST 8
//#define TFT_LED 0   // 0 if wired to +5V directly
//
//TFT_22_ILI9225 tft = TFT_22_ILI9225(TFT_RST, TFT_RS, TFT_CS, TFT_SDI, TFT_CLK, TFT_LED);

// Time
#include <RTCZero.h>

RTCZero rtc;
int lastSecond = 0; // rtc.getSeconds()' previous value
String  timeStamp;

// Nano 33 IoT Interuots: 2, 3, 9, 10, 11, 13, 15, A5, A7
const int setBtn = 9;
const int upBtn = 3;
const int downBtn = 2;

// config state
boolean configMode = false;

// relative time diff
int diff = 12; // default is 12 hours ahead

void setup() {
  Serial.begin(9600);

  // init display
  tft.begin();
  tft.setFont(Terminal6x8);
  
  // init RTC
  rtc.begin();
  rtc.setTime(0, 0, 0);

  pinMode(setBtn, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(setBtn), handleSet, FALLING);
  pinMode(upBtn, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(upBtn), handleUp, FALLING);
  pinMode(downBtn, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(downBtn), handleDown, FALLING);
}

void loop() {
  // check to see if in config mode
  if (configMode == 1) {
    configureClock();
  }

  // check time has elapsed
  if (rtc.getSeconds() != lastSecond) {
    displayTime(createTimeStamp());

    lastSecond = rtc.getSeconds();
  }
}

//  NOTE: -----> Clock Config <-----

void configureClock() {
  configHours();
  configMinutes();
  configDiff();
}

void configHours() {
  int hours = rtc.getHours();
}

void configMinutes() {
  int hours = rtc.getMinutes();
}

void configDiff() {

}

// NOTE: -----> Button Handlers <-----

void handleSet() {
  Serial.println("SET");
}

void handleUp() {
  Serial.println("UP");
}

void handleDown() {
  Serial.println("DOWN");
}

// NOTE: -----> Timestamp <-----

String createTimeStamp() {
  timeStamp = String(rtc.getHours()) + ':' + String(rtc.getMinutes()) + ':' + String(rtc.getSeconds());

  return timeStamp;
}

void displayTime(String time) {
  Serial.println(time);

  tft.drawText(10, 10, time);
}
