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

// clock
#include <RTCZero.h>

RTCZero rtc;
int lastSecond = 0; // rtc.getSeconds()' previous value
String  timeStamp;

// config state
boolean configMode = false;

// relative time diff
int diff = 12; // default is 12 hours ahead

// display
//#include "SPI.h"
//#include "TFT_22_ILI9225.h"
//
//#define USE_ARDUINO_INTERRUPTS true
//#define TFT_RST A4
//#define TFT_RS  A3
//#define TFT_CS  A5  // SS
//#define TFT_SDI A2  // MOSI
//#define TFT_CLK A1  // SCK
//#define TFT_LED 0   // 0 if wired to +5V directly

//TFT_22_ILI9225 tft = TFT_22_ILI9225(TFT_RST, TFT_RS, TFT_CS, TFT_SDI, TFT_CLK, TFT_LED);

void setup() {
  Serial.begin(9600);

  // init RTC
  rtc.begin();
  rtc.setTime(0, 0, 0);

  //  btn 1
  pinMode(D12, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(D12), handleBtn1, FALLING);
  
  // btn 2
  pinMode(D11, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(D11), handleBtn2, FALLING);
}

void loop() {
  // check to see if in config mode
  if (configMode == 1) {
    configureSystem();
  }

  // check time has elapsed
  if (rtc.getSeconds() != lastSecond) {
    displayTime(createTimeStamp());

    lastSecond = rtc.getSeconds();
  }
}

void configureSystem() {
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

String createTimeStamp() {
  timeStamp = String(rtc.getHours()) + ':' + String(rtc.getMinutes()) + ':' + String(rtc.getSeconds());

  return timeStamp;
}

void displayTime(String time) {
  Serial.println(time);
}
