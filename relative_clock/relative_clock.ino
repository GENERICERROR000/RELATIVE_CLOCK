/*
   Digital Clock with Relative Time

   When device is upside down, the time rotates
   180 degrees and shows a relative time.
   The relative time is set when the time is set.
   Relative time can be +/- hours:minutes.
   Time is set by using momentary push buttons.

   created 02/02/2020
   edited 02/12/2020
   Noah Kernis
*/

// display
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA A4, SCL SDL pins)
#define OLED_RESET 4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// IMU
#include <Arduino_LSM6DS3.h>

float x, y, z;

// time
#include <RTCZero.h>

RTCZero rtc;
int lastSecond = 0; // rtc.getSeconds()' previous value

// buttons
// interrupt pins: 2, 3, 9, 10, 11, 13, 15, A5, A7
const int setBtnPin = 9;
const int upBtnPin = 3;
const int downBtnPin = 2;

#include "OneButton.h"
OneButton setBtn(setBtnPin, true);
OneButton upBtn(upBtnPin, true);
OneButton downBtn(downBtnPin, true);

// config
boolean configMode = false;
boolean upsideDown = false;

// 0: hours, 1: minutes, 2: diffHours, 3: diffMinutes
int currentConfig = 0;

// relative time diff
// default is 12 hours 0 minutes ahead
int diffHours = 0;

// NOTE: -----> Setup <-----

void setup() {
  Serial.begin(9600);

  // init display
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }

  // init IMU
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");

    while (1);
  }

  // init RTC
  rtc.begin();
  rtc.setTime(0, 0, 0);

  // set button
  pinMode(setBtnPin, INPUT_PULLUP);
  setBtn.attachClick(setPressed);
  setBtn.attachDoubleClick(setDoublePressed);
  setBtn.setDebounceTicks(80);

  // up button
  pinMode(upBtnPin, INPUT_PULLUP);
  upBtn.attachClick(upPressed);

  // down button
  pinMode(downBtnPin, INPUT_PULLUP);
  downBtn.attachClick(downPressed);
}

// NOTE: -----> Loop <-----

void loop() {
  // keep watching the push button
  setBtn.tick();
  upBtn.tick();
  downBtn.tick();

  // check time has elapsed
  if (rtc.getSeconds() != lastSecond) {
    timeAction(0);

    lastSecond = rtc.getSeconds();
  }

  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(x, y, z);

    isUpsideDown(x);
  }
}

// NOTE: -----> Upside Down <-----

void isUpsideDown(int xPostion) {
  if (upsideDown) {
    if (xPostion > 0) upsideDown = !upsideDown;
  } else {
    if (xPostion < 0) upsideDown = !upsideDown;
  }
}

// NOTE: -----> Action Handler <-----

void setDoublePressed() {
  if (!upsideDown) timeAction(1);
}

void setPressed() {
  if (configMode == true && !upsideDown) timeAction(2);
}

void upPressed() {
  if (configMode == true && !upsideDown) timeAction(3);
}

void downPressed() {
  if (configMode == true && !upsideDown) timeAction(4);
}

// NOTE: -----> Timestamp <-----

String createTimeStamp() {
  int currentHours = rtc.getHours();
  String currentMinutes = needsAZero(rtc.getMinutes());

  if (upsideDown) {
    return needsAZero(handleDiffHours(currentHours)) + ':' + currentMinutes;
  }
  return needsAZero(currentHours) + ':' + currentMinutes;
}

String needsAZero(int timeUnit) {
  if (timeUnit < 10  && !(timeUnit < 0)) {
    return "0" + String(timeUnit);
  }

  return String(timeUnit);
}

int handleDiffHours(int currentHours) {
  int x = currentHours + diffHours;

  if (diffHours > 0) {
    if (x > 23) {
      return x - 24;
    }
  } else {
    if (x < 0) {
      return x + 24;
    }
  }

  return x;
}

void displayTime(String time) {
  display.clearDisplay();

  if (upsideDown && !configMode) {
    display.setRotation(2);
  } else {
    display.setRotation(0);
  }

  if (configMode) {
    display.setCursor(0, 0);
    display.setTextColor(WHITE);
    display.setTextSize(1);
    display.print(currentlySetting());
  }

  if (upsideDown && !configMode) {
    display.setRotation(2);
    display.setCursor(4, 20);
  } else if (currentConfig == 2 && configMode) {
    display.setRotation(0);
    display.setCursor(40, 15);
  } else {
    display.setRotation(0);
    display.setCursor(4, 15);
  }

  display.setTextColor(WHITE);
  display.setTextSize(4);
  display.print(time);
  display.display();
}

//  NOTE: -----> Clock Config <-----

String currentlySetting() {
  switch (currentConfig) {
    case 0:
      return "HOURS";
    case 1:
      return "MINUTES";
    case 2:
      return "DIFFERENCE";
  }
}

void timeAction(int action) {
  switch (action) {
    case 0: // TIME
      break;
    case 1:
      if (configMode) currentConfig = 0;
      configMode = !configMode;
      break;
    case 2:
      handleSet();
      break;
    case 3:
      handleUp();
      break;
    case 4:
      handleDown();
      break;
  }

  if (currentConfig == 0 || currentConfig == 1) {
    displayTime(createTimeStamp());
  } else {
    displayTime(String(needsAZero(diffHours)));
  }
}

void handleSet() {
  if (currentConfig == 2) {
    currentConfig = 0;
    configMode = false;
  } else {
    currentConfig++;
  }
}

void handleUp() {
  int hrs = rtc.getHours();
  int mins = rtc.getMinutes();

  switch (currentConfig) {
    case 0:
      if (hrs == 23) hrs = 0;
      if (hrs < 23) hrs++;
      rtc.setHours(hrs);
      break;
    case 1:
      if (mins == 59) mins = 0;
      if (mins < 59) mins++;
      rtc.setMinutes(mins);
      break;
    case 2:
      if (diffHours == 23) diffHours = -23;
      if (diffHours < 23) diffHours++;
      break;
  }
}

void handleDown() {
  int hrs = rtc.getHours();
  int mins = rtc.getMinutes();

  switch (currentConfig) {
    case 0:
      if (hrs == 0) hrs = 23;
      if (hrs > 0) hrs--;
      rtc.setHours(hrs);
      break;
    case 1:
      if (mins == 0) mins = 59;
      if (mins > 0) mins--;
      rtc.setMinutes(mins);
      break;
    case 2:
      if (diffHours == -23) diffHours = 23;
      if (diffHours <= 23) diffHours--;
      break;
  }
}
