/**
 * @file   sine_wave
 * @Author Kevin Cocco (kevin@ardusat.com)
 * @date   April 29, 2015
 * @brief  plot a sine wave and use the generic valueToJSON formating from Ardusat experiment platform.
 *         Requires no wiring of sensors to demonstrate the graphing, logging and Theremin view
 */

#include <Arduino.h>
#include <Wire.h>
#include <ArdusatSDK.h>
#include <math.h>

ArdusatSerial serialConnection(SERIAL_MODE_HARDWARE_AND_SOFTWARE, 8,9);
float x;
float y;

void setup()
{ 
  serialConnection.begin(9600);
}

void loop()
{ 
  for(x=-PI;x<=PI;x+=0.05)  // loop through angles in radians
  {
    y = sin(x);
    serialConnection.println(valueToJSON("Sin",0,y));
    delay(5);
  }
}
