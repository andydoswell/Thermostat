/* James' Thermostat controller
   Copyright A.G.Doswell May 2017
   License: The MIT License (See full license at the bottom of this file)
   Visit Andydoz.blogspot.com for description and circuit.

*/


#include <OneWire.h> // from http://playground.arduino.cc/Learning/OneWire
#include <DallasTemperature.h> // from http://www.hacktronics.com/code/DallasTemperature.zip. When adding this to your IDE, ensure the .h and .cpp files are in the top directory of the library.
#include <EEPROM.h>
#include "LedControl.h"
LedControl lc = LedControl(12, 11, 10, 1); //Tells LedControl where our hardware is connected.
#define ONE_WIRE_BUS 8 // Data wire from temp sensor is plugged into pin 8 on the Arduino
OneWire oneWire(ONE_WIRE_BUS); // Setup a oneWire instance to communicate with any OneWire devices
DallasTemperature sensors(&oneWire); // Pass our oneWire reference to Dallas Temperature.

// define constants & global variables
const int rlyPin = 3; //defines pin the relay is connected to. relay is active low
int Timer = 0; //timer is off to start
const int upPin = 6;
const int downPin = 9;
int setPoint; // temperature set point
int setPointTimer = 0; // timer to prevent setPointAdjust loop from returning to main loop
int setPointEEPROM; // Set point value stored in EEPROM
int tempC; // Temperature in degrees C
boolean heaterOn; // true if heater on

void setup()
{

  lc.shutdown(0, false); // Wake up the MAX 72xx controller
  lc.setIntensity(0, 8); // Set the display brightness
  lc.clearDisplay(0); //Clear the display
  pinMode (rlyPin, OUTPUT); // sets our relay pin
  digitalWrite (rlyPin, LOW); // sets the relay off for default condition.
  pinMode (upPin, INPUT_PULLUP); // sets up control pin
  pinMode (downPin, INPUT_PULLUP); // sets down control pin
  setPoint = EEPROM.read(0); // get the stored set point
  sensors.begin(); // start up the Dallas temperature sensors
}

void loop()
{
  setPointAdjust (); // calls the adjust set point routine
  sensors.requestTemperatures(); // Request temperature
  tempC = sensors.getTempCByIndex(0); // set the temperature in degrees C from the first sensor in line, rather than by address.
  updateDisplay (); // update the display
  if (tempC >= (setPoint)) { // if the temperature is greater than or at the setpoint, switch the heater off
    digitalWrite (rlyPin, LOW); // switch heater off
    heaterOn = false;// set the flag (used in updateDisplay)
  }
  else {
    digitalWrite (rlyPin, HIGH);// switch heater on
    heaterOn = true;
  }
}

void setPointAdjust () { // check the up down controls, and adjust the set point as required

  if (digitalRead (upPin) == false) { // if the up switch is pressed, increment the set point
    setPoint++;
    if (setPoint >= 101) { // limit the set point to 100 degrees
      setPoint = 100;
    }
    updateDisplay ();
    delay (200); // sensible delay to stop the set point being incremented too fast
    setPointTimer = 100; // delay number of loops before returning to main loop
  }
  if (digitalRead (downPin) == false) { // same as above for decrement
    setPoint--;
    if (setPoint <= 0 ) { // minimum set point is 1 degree
      setPoint = 1;
    }
    updateDisplay ();
    delay (200);
    setPointTimer = 100;
  }

  if (setPointTimer == 0) { // delay return to loop for 100 operations, then write to EEPROM and return to main loop
    setPointEEPROM = EEPROM.read(0);
    if (setPointEEPROM != setPoint) { // if the value changed, write new value to the EEPROM
      EEPROM.write(0, setPoint);

    }
    return;
  }

  setPointTimer --;
  delay (50);
  setPointAdjust (); // loop back round unit the setPointTimer has expired
}

void updateDisplay () { // update the LED display
  int tempUnit = (tempC % 10); // units of temp
  int tempTen = (tempC / 10) % 10; // tens of temp
  int setPointUnit = (setPoint % 10); // units of setpoint
  int setPointTen = (setPoint / 10) % 10; // tens of setpoint
  lc.setDigit (0, 7, tempTen, false); // write info to display
  lc.setDigit (0, 6, tempUnit, false);
  lc.setRow(0, 5, B01100011); //degree symbol
  lc.setRow(0, 4, B01001110); // display C
  lc.setDigit (0, 3, setPointTen, false);
  lc.setDigit (0, 2, setPointUnit, false);
  lc.setRow(0, 0, B01001110); // display C
  if (heaterOn == true) { // set decimal place on if the heater is on, on last display.
    lc.setRow(0, 1, B11100011); // degree symbol with decimal place set

  }
  else {
    lc.setRow(0, 1, B01100011); //degree symbol
  }

}
/*
   Copyright (c) 2017 Andrew Doswell

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute and sublicense
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   Any commercial use is prohibited without prior arrangement.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHOR(S) OR COPYRIGHT HOLDER(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
   THE SOFTWARE.
*/
