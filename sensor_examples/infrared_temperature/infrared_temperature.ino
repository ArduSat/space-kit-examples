/*
 * =====================================================================================
 *
 *       Filename:  infrared_temperature.ino
 *
 *    Description:  Sample example sketch that reads the infrared temperature using the 
 *                  MLX90614 IR Thermopile and turns on an LED if the temp rises above 
 *                  about 85 F.
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
#include <Adafruit_MLX90614.h>

#define READ_INTERVAL 100 // interval, in ms, to wait between readings
#define TEMP_CELCIUS 1 // Read temperatures in Celcius
#define TEMP_FAHRENHEIT 1 // Read temperatures in Fahrenheit
#define TEMP_OUTPUT TEMP_FAHRENHEIT

#define LED_MLX90614 2 // Light turns on based on infrared temp

//https://github.com/adafruit/Adafruit-MLX90614-Library
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  error_initializing_sensor
 *  Description:  Helper function to handle a sensor that doesn't start up properly.
 *                Prints an error message for diagnostic purposes, then stops program
 *                execution. This version takes a string sensor name.
 * =====================================================================================
 */
void error_initializing_sensor(char * name)
{
  Serial.print("Ooops, no ");
  Serial.print(name); 
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
void write_json_value ( char *sensor_name, char *units, double value)
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
 *         Name:  setup
 *  Description:  This function runs when the Arduino first turns on/resets. This is 
 *                our chance to take care of all one-time configuration tasks to get
 *                the program ready to begin logging data.
 * =====================================================================================
 */
void setup() {
  Serial.begin(9600);
  Serial.println("Ardusat IR Thermopile Demo"); 
  
  // Initialize the sensors that use the Adafruit Unified Sensor Driver
  if (!mlx.begin()) {
    error_initializing_sensor("MLX90614");
  }
  
  // initialize the digital pins as outputs for the LEDs
  pinMode(LED_MLX90614, OUTPUT);       
  
  /* We're ready to go! */
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
  double infrared_temp;
  char *temp_unit;

  // Read MLX Infrared temp sensor
  if (TEMP_OUTPUT == TEMP_FAHRENHEIT) {
    infrared_temp = mlx.readAmbientTempF();
    temp_unit = "F";
  } else {
    infrared_temp = mlx.readAmbientTempC();
    temp_unit = "C";
  }
  write_json_value("infraredAmbient", temp_unit, infrared_temp);

  if (TEMP_OUTPUT == TEMP_FAHRENHEIT) {
    infrared_temp = mlx.readObjectTempF();
    if (infrared_temp > 85) {
      digitalWrite(LED_MLX90614, HIGH);   // turn the LED on (HIGH is the voltage level)
    } else {
      digitalWrite(LED_MLX90614, LOW);    // turn the LED off by making the voltage LOW
    }
  } else {
    infrared_temp = mlx.readObjectTempC();
    if (infrared_temp > 29.5) {
      digitalWrite(LED_MLX90614, HIGH);   // turn the LED on (HIGH is the voltage level)
    } else {
      digitalWrite(LED_MLX90614, LOW);    // turn the LED off by making the voltage LOW
    }
  }
  write_json_value("infraredObject", temp_unit, infrared_temp);
       
  delay(READ_INTERVAL);
}
