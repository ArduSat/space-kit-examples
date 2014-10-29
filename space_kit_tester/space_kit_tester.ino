/*
 * =====================================================================================
 *
 *       Filename:  space_kit_tester.ino
 *
 *    Description:  Simple driver for all the sensors included in the Ardusat
 *                  Space Kit. Outputs all sensor values at a configurable 
 *                  interval in JSON format that can be read by the Ardusat 
 *                  demo app (https://demo.ardusat.com).
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

//IMU LSM303/l3GD20
#include <Adafruit_LSM303_U.h>
#include <Adafruit_L3GD20_U.h>
//#include <Adafruit_9DOF.h>

//TMP102 Temp
#include <tmp102.h>

//MLX90614 IR Thermopile
#include <Adafruit_MLX90614.h>

//TSL2561 Lux
#include <Adafruit_TSL2561_U.h>

/*-----------------------------------------------------------------------------
 *  Constant Definitions
 *-----------------------------------------------------------------------------*/
#define READ_INTERVAL 100 // interval, in ms, to wait between readings
#define TEMP_CELCIUS 1 // Read temperatures in Celcius
#define TEMP_FAHRENHEIT 1 // Read temperatures in Fahrenheit
#define TEMP_OUTPUT TEMP_FAHRENHEIT

// Pin Definitions
// 3 LEDs can be turned on/off based on sensor values. These constants define the pins
// they are attached to
#define LED_TMP102 3 // Light turns on based on temp sensor
#define LED_MLX90614 2 // Light turns on based on infrared temp
#define LED_TSL2561 4 // Light turns on based on low light conditions
#define LED_SERIAL 5

// These pins are used for measurements
#define LDR_Pin A2 // analog pin 2 for LDR photo sensor
#define UVOUT A0 //Output from the UV sensor
#define REF_3V3 A1 // 3.3V power on the Arduino board

/*-----------------------------------------------------------------------------
 *  Sensor Object Definitions
 *-----------------------------------------------------------------------------*/
// A number of sensors have drivers that extend Adafruit's Unified Sensor framework
// (https://github.com/adafruit/Adafruit_Sensor)
Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(30301);
Adafruit_LSM303_Mag_Unified mag   = Adafruit_LSM303_Mag_Unified(30302);
Adafruit_L3GD20_Unified gyro  = Adafruit_L3GD20_Unified(20);
Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);

// Other sensors have their own unique drivers. Go to the sensor's github repository to get
// usage details
//https://www.sparkfun.com/products/11931
#define TMP102_ADDR 0x48
Tmp102 tmp102(&Wire, TMP102_ADDR);

//https://github.com/adafruit/Adafruit-MLX90614-Library
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

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
  error_initializing_sensor(sensor.name);
}		/* -----  end of function error_initializing_sensor  ----- */

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
void write_json_value ( char *sensor_name, char *units, int value)
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
void write_json_value ( char *sensor_name, char *units, double value)
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
  Serial.println("Ardusat Space Kit tester"); 
  
  // Initialize the sensors that use the Adafruit Unified Sensor Driver
  if (!accel.begin()) {
    error_initializing_sensor(accel);
  }
  if (!mag.begin()) {
    error_initializing_sensor(mag);
  }
  if (!gyro.begin()) {
    error_initializing_sensor(gyro);
  }
  if (!mlx.begin()) {
    error_initializing_sensor("MLX90614");
  }
  if (!tsl.begin()) {
    error_initializing_sensor(tsl);
  }
  configure_tsl(&tsl, true);
  
  // initialize the digital pins as outputs for the LEDs
  pinMode(LED_TMP102, OUTPUT);
  pinMode(LED_MLX90614, OUTPUT);       
  pinMode(LED_TSL2561, OUTPUT);
  pinMode(LED_SERIAL, OUTPUT); 
  
  // More pin initializations
  pinMode(UVOUT, INPUT);
  pinMode(REF_3V3, INPUT);
  
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
  byte byteRead;
  sensors_event_t event;
  int ldr_value;
  float uv_intensity;
  float temp;
  char * temp_unit;
  double infrared_temp;

  // To test sending serial data from the computer, we can turn the Serial Read
  // LED on or off
  // Entering 1 will turn ON, 0 or any other number turns OFF
  if (Serial.available()) {
    byteRead = Serial.read();

    // Echo the value read back on the serial port
    Serial.write(byteRead);
    if (byteRead == 49) // Equals 1 / ON
    {
      digitalWrite(LED_SERIAL, HIGH);   // turn the LED on (HIGH is the voltage level)
    }
    else
    {
      digitalWrite(LED_SERIAL, LOW);    // turn the LED off by making the voltage LOW
    }  
  }

  // Read Photoresistor
  ldr_value = analogRead(LDR_Pin);
  write_json_value("photoresistor", "Lux", ldr_value);

  // Read Accelerometer
  accel.getEvent(&event);
  write_json_value("accelerometerX", "m/s^2", event.acceleration.x);
  write_json_value("accelerometerY", "m/s^2", event.acceleration.y);
  write_json_value("accelerometerZ", "m/s^2", event.acceleration.z);

  // Read Magnetometer
  mag.getEvent(&event);
  write_json_value("magnetometerX", "uT", event.magnetic.x);
  write_json_value("magnetometerY", "uT", event.magnetic.y);
  write_json_value("magnetometerZ", "uT", event.magnetic.z);
  
  // Read Gyro
  gyro.getEvent(&event);
  write_json_value("gyroX", "rad/s", event.gyro.x);
  write_json_value("gyroY", "rad/s", event.gyro.y);
  write_json_value("gyroZ", "rad/s", event.gyro.z);

  // Read Temp from TMP102 (default in celcius)
  temp = tmp102.read();

  // Logic to turn on the temperature LED based on detected temp above ~29.5 C / 85 F
  if (temp > 29.5) {
    digitalWrite(LED_TMP102, HIGH);   // turn the LED on (HIGH is the voltage level)
  } else {
    digitalWrite(LED_TMP102, LOW);    // turn the LED off by making the voltage LOW
  }
  temp_unit = "C";
  if (TEMP_OUTPUT == TEMP_FAHRENHEIT) {
    temp = temp * 9 / 5 + 32;
    temp_unit = "F";
  }
  write_json_value("temp", temp_unit, temp);
  
  // Read MLX Infrared temp sensor
  if (TEMP_OUTPUT == TEMP_FAHRENHEIT) {
    infrared_temp = mlx.readAmbientTempF();
  } else {
    infrared_temp = mlx.readAmbientTempC();
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
  
  // Read MP8511 UV 
  uv_intensity = readMP8511();
  write_json_value("UV", "mW/cm^2", uv_intensity);
       
  delay(READ_INTERVAL);
}
