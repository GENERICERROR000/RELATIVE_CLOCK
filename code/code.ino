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

// NOTE: REFS:
// - https://github.com/mathertel/OneButton/blob/master/examples/SimpleOneButton/SimpleOneButton.ino
// - https://startingelectronics.org/tutorials/arduino/modules/OLED-128x64-I2C-display/
// - https://learn.adafruit.com/adafruit-gfx-graphics-library/graphics-primitives
// - https://github.com/arduino-libraries/Arduino_LSM6DS3

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

// time
#include <RTCZero.h>

RTCZero rtc;
int lastSecond = 0; // rtc.getSeconds()' previous value

// buttons
// interrupt pins: 2, 3, 9, 10, 11, 13, 15, A5, A7
const int setBtn = 9;
const int upBtn = 3;
const int downBtn = 2;

#include "OneButton.h"
OneButton button(setBtn, true);

// config
boolean configMode = false;
boolean upsideDown = false;

// 0: hours, 1: minutes, 2: diffHours, 3: diffMinutes
int currentConfig = 0;

int debounce = 6;

// relative time diff
// default is 12 hours 0 minutes ahead
int diffHours = 12;
int diffMinutes = 0;

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

  float x, y, z;

  if (IMU.gyroscopeAvailable()) {
//    IMU.readGyroscope(x, y, z);

//    Serial.print(x);
//    Serial.print('\t');
//    Serial.print(y);
//    Serial.print('\t');
//    Serial.println(z);
  }
}

// NOTE: -----> Action Handler <-----

void setDoublePressed() {
  delay(debounce);
  timeAction(1);
}

void setPressed() {
  delay(debounce);
  if (configMode == true) timeAction(2);
}

void upPressed() {
  delay(debounce);
  if (configMode == true) timeAction(3);
}

void downPressed() {
  delay(debounce);
  if (configMode == true) timeAction(4);
}

// NOTE: -----> Timestamp <-----

String createTimeStamp() {
  if (upsideDown) {
    // TODO: Need to do time math - can't just add together
    return String(rtc.getHours() + diffHours) + ':' + String(rtc.getMinutes() + diffHours);
  } else {
    return String(rtc.getHours()) + ':' + String(rtc.getMinutes());
  }
}

void displayTime(String time) {
//  Serial.println(time);
  display.clearDisplay();

  display.setCursor(40, 20);
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.print(time);
  
  display.display();
}

//  NOTE: -----> Clock Config <-----

void timeAction(int action) {
  switch (action) {
    case 0: // TIME
      break;
    case 1:
      Serial.println("DOUBLECLICK");
      configMode = !configMode;

      break;
    case 2:
      Serial.println("SET");
      handleSet();

      break;
    case 3:
      Serial.println("UP");
      handleUp();

      break;
    case 4:
      Serial.println("DOWN");
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
