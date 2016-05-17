/*
 * =====================================================================================
 *
 *       Filename:  magnet_strum.ino
 *
 *    Description:  Simple example for using the Space Kit to play audio using the 
 *                  Sensor Drum Kit Example
 *                  http://experiments.ardusat.com/#/sensors/drum-kit
 *
 *                  Example plays a sitar strum sound when within the presence of
 *                  a strong enough magnetic force. The magnetometer was placed
 *                  above a rig with a magnet taped to the center of a stretched
 *                  rubber band.
 *
 *                  This example uses many third-party libraries available from
 *                  Adafruit (https://github.com/adafruit). These libraries are
 *                  mostly under an Apache License, Version 2.0.
 *
 *                  http://www.apache.org/licenses/LICENSE-2.0
 *
 *        Version:  1.0
 *        Created:  5/14/2015
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
#include <Math.h>

/*-----------------------------------------------------------------------------
 *  Setup Software Serial to allow for both RF communication and USB communication
 *    RX is digital pin 10 (connect to TX/DOUT of RF Device)
 *    TX is digital pin 11 (connect to RX/DIN of RF Device)
 *-----------------------------------------------------------------------------*/
ArdusatSerial serialConnection(SERIAL_MODE_HARDWARE_AND_SOFTWARE, 8, 9);

/*-----------------------------------------------------------------------------
 *  Constant Definitions
 *-----------------------------------------------------------------------------*/
const int BEAT = 75;        // Time, in ms, to wait between logging
unsigned long lastBeat = 0;
Magnetic mag;


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
  mag.begin();

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
  float mag_mag;

  // Read Magnetometer
  mag.read();

  // Calculate the magnetic magnitude
  mag_mag = sqrt(mag.x * mag.x + mag.y * mag.y + mag.z * mag.z);

  // Play the sound only once per BEAT when the magnetic magnitude is high enough
  if (mag_mag > 111.0 && (millis() - lastBeat) > BEAT) {
    serialConnection.println(valueToJSON("sitar1", DATA_UNIT_NONE, 1));
    lastBeat = millis();
  }
}
