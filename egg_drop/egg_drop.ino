/*
 * =====================================================================================
 *
 *       Filename:  egg_drop.ino
 *
 *    Description:  Logs accelerometer data to an SD card for use with an egg
 *                  drop experiment.
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
 *         Author:  Chris Hoffman (chris@ardusat.com)
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
#include <ArdusatLogging.h>

/*-----------------------------------------------------------------------------
 *  Setup Software Serial to allow for both RF communication and USB communication
 *    RX is digital pin 10 (connect to TX/DOUT of RF Device)
 *    TX is digital pin 11 (connect to RX/DIN of RF Device)
 *-----------------------------------------------------------------------------*/
ArdusatSerial serialConnection(SERIAL_MODE_HARDWARE_AND_SOFTWARE, 8, 9);

/*-----------------------------------------------------------------------------
 *  Constant Definitions
 *-----------------------------------------------------------------------------*/
const short SD_CS_PIN = 10;     // pin used for SD card reader
const char LOG_FILE_PREFIX[] = "MYLOG";
const bool LOG_CSV_DATA = true; // otherwise log binary data to save space
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

  if (!beginDataLog(SD_CS_PIN, LOG_FILE_PREFIX, LOG_CSV_DATA)) {
    serialConnection.println("Failed to initialize SD card...");
    while (true);
  }

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

  if (LOG_CSV_DATA) {
    logAcceleration("accelerometer", accel);
  } else {
    binaryLogAcceleration(0, accel);
  }
}
