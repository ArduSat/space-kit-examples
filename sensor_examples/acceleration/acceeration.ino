/*
 * =====================================================================================
 *
 *       Filename:  acceleration.ino
 *
 *    Description:  Demo looks at the 3 axis acceleration readings from the LSM303 
 *                  accelerometer and outputs them at at a configurable frequency in a
 *                  JSON format that can be read by the Ardusat demo app 
 *                  (https://demo.ardusat.com).
 *
 *                  This example uses many third-party libraries available from
 *                  Adafruit (https://github.com/adafruit). These libraries are
 *                  mostly under an Apache License, Version 2.0.
 *
 *                  http://www.apache.org/licenses/LICENSE-2.0
 *
 *        Version:  1.0
 *        Created:  10/29/2014 02:50:13
 *       Revision:  none
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
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>

/*-----------------------------------------------------------------------------
 *  Definitions
 *-----------------------------------------------------------------------------*/
#define READ_INTERVAL 100 // interval, in ms, to wait between readings
Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(30301);

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  error_initializing_sensor
 *  Description:  Helper function to handle a sensor that doesn't start up properly.
 *                Prints an error message for diagnostic purposes, then stops program
 *                execution. 
 * =====================================================================================
 */
void error_initializing_sensor(Adafruit_Sensor & sensor_c)
{
  sensor_t sensor;
  sensor_c.getSensor(&sensor);

  Serial.print("Ooops, no ");
  Serial.print(sensor.name); 
  Serial.println("detected! Check your wiring...");
  while (1);
}		/* -----  end of function error_initializing_sensor  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  write_json_value
 *  Description:  Writes a JSON encoded string over the serial line for the computer to
 *                read. There are multiple versions of this function to handle the different
 *                possible value types from the various functions.
 * =====================================================================================
 */
void write_json_value ( char *sensor_name, char *units, float value)
{
  Serial.println("~");
  Serial.print("{\"sensorName\":\"");
  Serial.print(sensor_name);
  Serial.print("\",\"unit\":\"");
  Serial.print(units);
  Serial.print("\",\"value\":");
  Serial.print(value);
  Serial.println("}|");
}		/* -----  end of function write_json_value  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  setup
 *  Description:  This function runs when the Arduino first turns on/resets. This is 
 *                our chance to take care of all one-time configuration tasks to get
 *                the program ready to begin logging data.
 * =====================================================================================
 */
void setup() {
  Serial.begin(9600);
  Serial.println("Ardusat Acceleration Sensor Demo"); 
  
  if (!accel.begin()) {
    error_initializing_sensor(accel);
  }

  /* We're ready to go! */
  Serial.println("Sensor initialized successfully, starting to read data...");
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
  sensors_event_t event;

  // Read Accelerometer
  accel.getEvent(&event);
  write_json_value("accelerometerX", "m/s^2", event.acceleration.x);
  write_json_value("accelerometerY", "m/s^2", event.acceleration.y);
  write_json_value("accelerometerZ", "m/s^2", event.acceleration.z);
  
  delay(READ_INTERVAL);
}
