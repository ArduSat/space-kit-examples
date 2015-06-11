/*
 * =====================================================================================
 *
 *       Filename:  IMU_all_sensors.ino
 *
 *    Description:  Simple driver for all the sensors included in the Ardusat
 *                  Space Kit. Outputs all sensor values at a configurable 
 *                  interval in JSON format that can be read by the Ardusat 
 *                  demo app (https://experiments.ardusat.com).
 *
 *                  This example uses many third-party libraries available from
 *                  Adafruit (https://github.com/adafruit). These libraries are
 *                  mostly under an Apache License, Version 2.0.
 *
 *                  http://www.apache.org/licenses/LICENSE-2.0
 *
 *        Version:  1.1
 *        Created:  10/29/2014 02:50:13
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

ArdusatSerial serialConnection(SERIAL_MODE_HARDWARE_AND_SOFTWARE, 8, 9);

/*-----------------------------------------------------------------------------
 *  Constant Definitions
 *-----------------------------------------------------------------------------*/
#define READ_INTERVAL 100 // interval, in ms, to wait between readings

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  setup
 *  Description:  This function runs when the Arduino first turns on/resets. This is 
 *                our chance to take care of all one-time configuration tasks to get
 *                the program ready to begin logging data.
 * =====================================================================================
 */
void setup() {
  serialConnection.begin(9600);
  serialConnection.println("Ardusat Space Kit tester");

  beginAccelerationSensor();
  beginGyroSensor();
  beginMagneticSensor();
  beginBarometricPressureSensor();

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
void loop() {
  temperature_t temp;
  acceleration_t accel;
  magnetic_t mag;
  gyro_t gyro;
  orientation_t orientation;
  pressure_t pressure;

  // Read Accelerometer
  readAcceleration(&accel);
  serialConnection.println(accelerationToJSON("accelerometer", &accel));

  // Read Magnetometer
  readMagnetic(&mag);
  serialConnection.println(magneticToJSON("magnetic", &mag));

  // Read Gyro
  readGyro(&gyro);
  serialConnection.println(gyroToJSON("gyro", &gyro));

  // Calculate Orientation from Accel + Magnet data
  calculateOrientation(&accel, &mag, &orientation);
  serialConnection.println(orientationToJSON("orientation", &orientation));
  
  // Read BMP180 Barometer Pressure 
  readBarometricPressure(&pressure);
  serialConnection.println(pressureToJSON("pressure", &pressure));

  delay(READ_INTERVAL);
}
