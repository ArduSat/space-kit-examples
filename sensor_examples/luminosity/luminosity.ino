/*
 * =====================================================================================
 *
 *       Filename:  luminosity.ino
 *
 *    Description:  Simple demo for measuring ambient light (luminosity) using the 
 *                  TSL2561 Luminosity sensor. When the measured light value drops
 *                  below 100 Lux (low light), the sketch will turn an LED on.
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

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561_U.h>

#define READ_INTERVAL 100 // interval, in ms, to wait between readings

#define LED_TSL2561 4 // Light turns on based on low light conditions

Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  configure_tsl
 *  Description:  Configures the gain and integration time for the TSL2561 Lux/Luminosity 
 *                Sensor. Gain can be set manually, or using the auto-gain function (default)
 *
 *                Integration time gives better sensor resolution but slower update speed
 *
 *                See Adafruit_TSL2561_U.h for complete listings of options.
 * =====================================================================================
 */
void configure_tsl ( Adafruit_TSL2561_Unified * tsl, bool auto_range, tsl2561Gain_t gain=TSL2561_GAIN_1X, 
                    tsl2561IntegrationTime_t integration_time=TSL2561_INTEGRATIONTIME_13MS)
{
  if (auto_range) {
    tsl->enableAutoRange(auto_range);
  } else {
    tsl->setGain(gain);
  }
  tsl->setIntegrationTime(integration_time);
}		/* -----  end of function configure_tsl  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  error_initializing_sensor
 *  Description:  Helper function to handle a sensor that doesn't start up properly.
 *                Prints an error message for diagnostic purposes, then stops program
 *                execution. This version takes a Adafruit_Sensor object.
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
  write_boilerplate(sensor_name, units);
  Serial.print(value);
  Serial.println("}|");
}		/* -----  end of function write_json_value  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  write_json_value
 *  Description:  Writes a JSON encoded string over the serial line for the computer to
 *                read. There are multiple versions of this function to handle the different
 *                possible value types from the various functions.
 * =====================================================================================
 */
void write_json_value ( char *sensor_name, char *units, char *value)
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
  Serial.println("Ardusat Luminosity Demo"); 
  
  if (!tsl.begin()) {
    error_initializing_sensor(tsl);
  }
  configure_tsl(&tsl, true);
  
  // initialize the digital pins as outputs for the LEDs
  pinMode(LED_TSL2561, OUTPUT);
  
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
  sensors_event_t event;

  // Read TSL2561 Luminosity
  tsl.getEvent(&event);
  // if event.light == 0, the lux sensor is probably saturated, so no reliable
  // data could be generated, write out a null value
  if (event.light) {
    write_json_value("luminosity", "Lux", event.light);
    if (event.light < 100) {
      digitalWrite(LED_TSL2561, HIGH);   // turn the LED on (HIGH is the voltage level)
    } else {
      digitalWrite(LED_TSL2561, LOW);    // turn the LED off by making the voltage LOW
    }
  } else {
    write_json_value("luminosity", "Lux", "null");
  }
       
  delay(READ_INTERVAL);
}
