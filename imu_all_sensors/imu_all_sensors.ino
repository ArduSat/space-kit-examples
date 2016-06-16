/*
 * =====================================================================================
 *
 *       Filename:  imu_all_sensors.ino
 *
 *    Description:  Simple driver for all the sensors included in the Ardusat
 *                  Space Kit. Outputs all sensor values at a configurable 
 *                  interval in JSON format that can be read by the Ardusat 
 *                  Experiment Platform  (https://experiments.ardusat.com).
 *
 *                  Example returns json values for all of the sensors available
 *                  from just the IMU.
 *
 *                  This example uses many third-party libraries available from
 *                  Adafruit (https://github.com/adafruit). These libraries are
 *                  mostly under an Apache License, Version 2.0.
 *
 *                  http://www.apache.org/licenses/LICENSE-2.0
 *
 *        Version:  1.1
 *        Created:  10/29/2014
 *       Revision:  5/18/2015 - add BMP180 pressure and temp
 *       Compiler:  Arduino
 *
 *         Author:  Ben Peters (ben@ardusat.com)
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
const short READ_INTERVAL = 100; // interval, in ms, to wait between readings
Acceleration accel;
Gyro gyro;
Magnetic mag;
Orientation orient(accel, mag);
Pressure press;
Temperature temp;

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

  accel.begin();
  gyro.begin();
  mag.begin();
  orient.begin();
  press.begin();
  temp.begin();

  /* We're ready to go! */
  serialConnection.println("");
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  loop
 *  Description:  After setup runs, this loop function runs until the Arduino loses 
 *                power or resets. We go through and update each of the attached sensors,
 *                write out the updated values in JSON format, then delay before repeating
 *                the loop again.
 * =====================================================================================
 */
void loop(void)
{
  // Read Accelerometer
  serialConnection.println(accel.readToJSON("accel"));

  // Read Gyro
  serialConnection.println(gyro.readToJSON("gyro"));

  // Read Magnetometer
  serialConnection.println(mag.readToJSON("mag"));

  // Calculate Orientation from Accel + Magnet data
  serialConnection.println(orient.readToJSON("orient"));
  
  // Read BMP180 Barometer Pressure 
  serialConnection.println(press.readToJSON("pressure"));

  // Read Ambient Temperature
  serialConnection.println(temp.readToJSON("temp"));

  delay(READ_INTERVAL);
}
