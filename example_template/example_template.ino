/*
 * =====================================================================================
 *
 *       Filename:  example_template.ino
 *
 *    Description:  A template for how to design an example sketch.
 *
 *                  Use the Ardusat Experiment Platform to visualize your data!
 *                  http://experiments.ardusat.com
 *
 *                  This example uses many third-party libraries available from
 *                  Adafruit (https://github.com/adafruit). These libraries are
 *                  mostly under an Apache License, Version 2.0.
 *
 *                  http://www.apache.org/licenses/LICENSE-2.0
 *
 *        Version:  1.0
 *        Created:  09/01/2015
 *
 *         Author:  Sam Olds (sam@ardusat.com)
 *   Organization:  Ardusat
 *
 * =====================================================================================
 */

/*-----------------------------------------------------------------------------
 *  Includes
 *-----------------------------------------------------------------------------*/
#include <Arduino.h>
#include <Wire.h>
#include <ArdusatSDK.h>

/*-----------------------------------------------------------------------------
 *  Setup Software Serial to allow for both RF communication and USB communication
 *    RX is digital pin 10 (connect to TX/DOUT of RF Device)
 *    TX is digital pin 11 (connect to RX/DIN of RF Device)
 *-----------------------------------------------------------------------------*/
ArdusatSerial serialConnection(SERIAL_MODE_HARDWARE_AND_SOFTWARE, 8, 9);

/*-----------------------------------------------------------------------------
 *  Constant Definitions
 *-----------------------------------------------------------------------------*/
const int BEAT = 500;           // Time, in ms, to wait between logging
acceleration_t accel;


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
  beginAccelerationSensor();

  /* We're ready to go! */
  serialConnection.println("");
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  loop
 *  Description:  After setup runs, this loop function runs until the Arduino loses 
 *                power or resets. We go through and read from each of the attached
 *                sensors, write out the corresponding sounds in JSON format, then
 *                delay before repeating the loop again.
 * =====================================================================================
 */
void loop(void)
{
  // Read Accelerometer
  readAcceleration(accel);

  if (accel.x > 5) {
    other_function("value1");
  } else if (accel.x < -5) {
    other_function("value2");
  } else {
    other_function("value3");
  }

  delay(BEAT);
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  other_function
 *  Description:  This function takes a string and writes it to the serial connection
 *                in JSON.
 *   Parameters:  string: thing to print to serial connection
 * =====================================================================================
 */
void other_function(const char * string)
{
  serialConnection.println(valueToJSON(string, DATA_UNIT_NONE, 1));
}
