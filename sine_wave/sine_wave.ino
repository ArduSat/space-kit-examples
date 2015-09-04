/*
 * =====================================================================================
 *
 *       Filename:  sine_wave.ino
 *
 *    Description:  Plot a sine wave and use the generic valueToJSON formating from
 *                  Ardusat Experiment Platform. Requires no wiring of sensors to
 *                  demonstrate the graphing, logging and Theremin view.
 *                  (http://experiments.ardusat.com)
 *
 *                  This example uses many third-party libraries available from
 *                  Adafruit (https://github.com/adafruit). These libraries are
 *                  mostly under an Apache License, Version 2.0.
 *
 *                  http://www.apache.org/licenses/LICENSE-2.0
 *
 *        Version:  1.0
 *        Created:  04/29/2015
 *       Revision:  none
 *       Compiler:  Arduino
 *
 *         Author:  Kevin Cocco (kevin@ardusat.com)
 *   Organization:  Ardusat
 *
 * =====================================================================================
 */

#include <Arduino.h>
#include <Wire.h>
#include <ArdusatSDK.h>
#include <math.h>

/*-----------------------------------------------------------------------------
 *  Setup Software Serial to allow for both RF communication and USB communication
 *    RX is digital pin 10 (connect to TX/DOUT of RF Device)
 *    TX is digital pin 11 (connect to RX/DIN of RF Device)
 *-----------------------------------------------------------------------------*/
ArdusatSerial serialConnection(SERIAL_MODE_HARDWARE_AND_SOFTWARE, 8, 9);

/*-----------------------------------------------------------------------------
 *  Constant Definitions
 *-----------------------------------------------------------------------------*/
float x;
float y;


/*
 * ===  FUNCTION  ======================================================================
 *         Name:  setup
 *  Description:  This function runs when the Arduino first turns on/resets. This is
 *                our chance to take care of all one-time configuration tasks to get
 *                the program ready to begin logging data.
 * =====================================================================================
 */
void setup(void)
{ 
  serialConnection.begin(9600);
}


/*
 * ===  FUNCTION  ======================================================================
 *         Name:  loop
 *  Description:  After setup runs, this loop function runs until the Arduino loses
 *                power or resets. We go through and update each of the attached
 *                sensors, write out the updated values in JSON format, then delay
 *                before repeating the loop again.
 * =====================================================================================
 */
void loop(void)
{ 
  // loop through angles in radians
  for (x = -PI; x <= PI; x += 0.05) {
    y = sin(x);
    serialConnection.println(valueToJSON("Sin", 0, y));
    delay(5);
  }
}
