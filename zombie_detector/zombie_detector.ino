/*
 * =====================================================================================
 *
 *       Filename:  zombie_detector.ino
 *
 *    Description:  Uses luminosity and infrared thermometer sensor readings to turn
 *                  an LED light on or off.
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
 *         Author:  Ben Peters (ben@email.com)
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
const int GREEN_LIGHT_PIN = 7;
const int RED_LIGHT_PIN = 9;

int baselineTemp = 0;
int baselineLight = 0;
luminosity_t lum;
temperature_t temp;


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
  pinMode(GREEN_LIGHT_PIN, OUTPUT);
  pinMode(RED_LIGHT_PIN, OUTPUT);

  if (!beginInfraredTemperatureSensor()) {
    serialConnection.println("Can't initialize IR temp sensor! Check your wiring.");
  }

  if (!beginLuminositySensor()) {
    serialConnection.println("Can't initialize luminosity sensor! Check your wiring.");
  }

  toggle_green_and_red_lights(true, true);

  readInfraredTemperature(temp);
  readLuminosity(lum);

  baselineLight = lum.lux;
  baselineTemp = temp.t;

  int i;
  for (i = 0; i < 100; i++) {
    readInfraredTemperature(temp);
    readLuminosity(lum);

    baselineLight += lum.lux;
    baselineTemp += temp.t;

    baselineLight /= 2;
    baselineTemp /= 2;
  }

  toggle_green_and_red_lights(false, false);

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
  // Read Temperature and Luminosity
  readInfraredTemperature(temp);
  readLuminosity(lum);

  serialConnection.println(temperatureToJSON("infared", temp));
  serialConnection.println(luminosityToJSON("luminosity", lum));

  if (lum.lux <= (baselineLight * .6)) {

    if (temp.t >= (baselineTemp * 1.05)) {
      toggle_green_and_red_lights(true, false);
    } else if (temp.t <= (baselineTemp * 1.03)) {
      toggle_green_and_red_lights(false, true);
    } else {
      toggle_green_and_red_lights(true, true);
    }

  } else if (lum.lux >= (baselineLight * .9)) {
    toggle_green_and_red_lights(false, false);
  }

  delay(100);
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  toggle_green_and_red_lights
 *  Description:  This function takes a string and writes it to the serial connection
 *                in JSON.
 *   Parameters:  green: Whether or not to turn the green led on or off
 *                red: Whether or not to turn the red led on or off
 * =====================================================================================
 */
void toggle_green_and_red_lights(boolean green, boolean red)
{
  if (green) {
    digitalWrite(GREEN_LIGHT_PIN, HIGH);
  } else {
    digitalWrite(GREEN_LIGHT_PIN, LOW);
  }

  if (red) {
    digitalWrite(RED_LIGHT_PIN, HIGH);
  } else {
    digitalWrite(RED_LIGHT_PIN, LOW);
  }
}
