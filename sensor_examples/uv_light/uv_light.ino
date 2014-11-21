/*
 * =====================================================================================
 *
 *       Filename:  uv_light.ino
 *
 *    Description:  UV Light demo using MP8511 UV Light sensor. This example outputs  
 *                  sensor values at a configurable interval in JSON format that can be
 *                  read by the Ardusat demo app (http://demo.ardusat.com)
 *
 *                  This example uses many third-party libraries available from
 *                  Adafruit (https://github.com/adafruit). These libraries are
 *                  mostly under an Apache License, Version 2.0.
 *
 *                  http://www.apache.org/licenses/LICENSE-2.0
 *
 *         Author:  Ben Peters (ben@ardusat.com)
 *
 * =====================================================================================
 */

/*-----------------------------------------------------------------------------
 *  Includes
 *-----------------------------------------------------------------------------*/
#include <Wire.h>

#define READ_INTERVAL 100 // interval, in ms, to wait between readings

#define UVOUT A0 //Output from the UV sensor
#define REF_3V3 A1 // 3.3V power on the Arduino board

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
  write_boilerplate(sensor_name, units);
  Serial.print(value);
  Serial.println("}|");
}		/* -----  end of function write_json_value  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  write_boilerplate
 *  Description:  Helper function to write out the common "first" part of the JSON message
 * =====================================================================================
 */
void write_boilerplate ( char *sensor_name, char *units )
{
  Serial.println("~");
  Serial.print("{\"sensorName\":\"");
  Serial.print(sensor_name);
  Serial.print("\",\"unit\":\"");
  Serial.print(units);
  Serial.print("\",\"value\":");
}		/* -----  end of function write_boilerplate  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  readMP8511
 *  Description:  Reads the MP8511 UV Sensor.
 *                To do this we take an average analog voltage read on the UV sensor pin
 *                and the 3.3 V power pin, then use that actual voltage to get a ratio of
 *                the read voltage compared to exactly 3.3V. We then map this value into
 *                a properly scaled value.
 * =====================================================================================
 */
float readMP8511 ()
{
  int uv_v = average_analog_read(UVOUT);
  int ref_v = average_analog_read(REF_3V3);
  float scaled_uv_v = 3.3 / ref_v * uv_v;
  return map_float(scaled_uv_v, 0.99, 2.9, 0.0, 15.0);
}		/* -----  end of function readMP8511  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  average_analog_read
 *  Description:  Takes 8 readings from the given pin then averages the values.
 * =====================================================================================
 */
int average_analog_read(int pinToRead)
{
  byte numberOfReadings = 8;
  unsigned int runningValue = 0; 

  for(int x = 0 ; x < numberOfReadings ; x++)
    runningValue += analogRead(pinToRead);
  runningValue /= numberOfReadings;

  return(runningValue);  
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  map_float
 *  Description:  Does a linear transform on the given value to come up with a scaled
 *                output.
 *                http://forum.arduino.cc/index.php?topic=3922.0
 * =====================================================================================
 */
float map_float(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

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
  Serial.println("Ardusat UV Light Demo"); 
  
  // More pin initializations
  pinMode(UVOUT, INPUT);
  pinMode(REF_3V3, INPUT);
  
  Serial.println("");
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
  float uv_intensity;
  
  // Read MP8511 UV 
  uv_intensity = readMP8511();
  write_json_value("UV", "mW/cm^2", uv_intensity);
       
  delay(READ_INTERVAL);
}
